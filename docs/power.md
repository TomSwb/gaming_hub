# Power (milestone 6, steps 1–4)

Show and switch the power profile over D-Bus, and alert when the charger comes out or the battery gets low during a session. Back to the [README](../README.md).

## The steps

| Step | What | Estimate | Actual |
| --- | --- | --- | --- |
| 1. Look by hand | `busctl` / `gdbus`: tuned-ppd, UPower, BlueZ | | ~0.2 h |
| 2. Power profile proxy | `GDBusProxy`, the profile on a label, kept live | 2 h | ~1 h |
| 3. Switching | One toggle button per profile, `Properties.Set`, no echo, errors | 2 h | ~1.1 h |
| 4. Battery alerts | Unplug and 25 % alerts during a session (planned with UPower, done from `/sys`) | 1–2 h | ~0.4 h |

## Look by hand (step 1, 22 Nov)

**Who is on the system bus** (`busctl list | grep -i -E 'power|bluez'`): tuned-ppd answers under **two names**, `org.freedesktop.UPower.PowerProfiles` (current) and `net.hadess.PowerProfiles` (old, kept for older programs): one program, two names. UPower is `org.freedesktop.UPower`, BlueZ is `org.bluez`. The hub uses the current power-profiles name.

**tuned-ppd's introspection is incomplete.** `busctl introspect` on `/org/freedesktop/UPower/PowerProfiles` lists only `HoldProfile`, `ReleaseProfile` and `ProfileReleased`, **no properties**, yet `busctl get-property … ActiveProfile` answers `s "balanced"`. tuned-ppd is written in Python (its errors say `DBus.Python.dbus.exceptions`). Rule: on this laptop, ask for a property directly rather than trusting the introspection list. `GDBusProxy` is fine with that: it loads properties with `GetAll`, not from the introspection data.

**`GetAll`** (`busctl --verbose call … org.freedesktop.DBus.Properties GetAll s org.freedesktop.UPower.PowerProfiles`):

| Property | Type | Value | For the hub |
| --- | --- | --- | --- |
| `ActiveProfile` | `s` | `balanced` | shown, and written to switch |
| `Profiles` | `aa{sv}` | 3 dictionaries: `{Profile: power-saver, Driver: tuned}`, `balanced`, `performance` | the buttons are built from it |
| `PerformanceDegraded` | `s` | `""` | showed `high-operating-temperature` while on Power Saver; not used yet |
| `ActiveProfileHolds`, `Actions`, `Version` (`0.23`) | | | not used |

**Signals** (`gdbus monitor --system --dest org.freedesktop.UPower.PowerProfiles`, switching in GNOME's menu): every change sends `PropertiesChanged` **twice**, once per name's object (`/org/freedesktop/...` and `/net/hadess/...`), with only the changed value: `('org.freedesktop.UPower.PowerProfiles', {'ActiveProfile': <'power-saver'>}, @as [])`. A proxy listens to one object, so it hears each change once.

**Writing** (`busctl set-property … ActiveProfile s power-saver`): no password, and GNOME's menu follows.

**UPower** (`busctl introspect org.freedesktop.UPower /org/freedesktop/UPower org.freedesktop.UPower`) is honest: `OnBattery` (`b`, `emits-change`), `LidIsClosed`, `LidIsPresent`, `DaemonVersion` 1.91.5. Unplugging and replugging send exactly one `PropertiesChanged` each (`OnBattery: true`, then `false`). Not used in the end, see step 4.

## Power profile proxy (step 2, 22 Nov)

- **`sources/power/`**, `includes/power.h`, a `t_power` in `t_hub` (label, row of buttons, proxy), set to `NULL` by `hub_power_init` (in `sources/inits_2.c`: `inits.c` already had 5 functions). GDBus is part of GIO, inside GTK4: no new package in the Makefile.
- **Async creation:** `g_dbus_proxy_new_for_bus` (system bus, `G_DBUS_PROXY_FLAGS_NONE` = load all properties and follow changes, no interface info, service / path / interface, `hub_power_ready` + `hub`), from `hub_activate`. The `_sync` version would freeze the window until the service answers.
- **`hub_power_ready`:** `g_dbus_proxy_new_for_bus_finish` gives the proxy (a new reference the hub owns, unref'd in `hub_free_hub`) or `NULL` + a `GError` (`error = NULL` before, `error->message` printed, `g_error_free`).
- **Reading the cache:** `g_dbus_proxy_get_cached_property` returns a `GVariant` you own (`g_variant_unref`) or `NULL`; `g_variant_get_string` returns a string borrowed from inside it (use before the unref, never free). Check the type first (`g_variant_is_of_type`), never unref `NULL`.
- **Live:** `g-properties-changed` on the proxy, connected in `hub_power_ready` (connecting right after the async request connected to `NULL`: the proxy didn't exist yet). The cache is updated before the signal, so the handler just calls `hub_power_show` again.
- **Tests:** `systemctl stop tuned-ppd` while running → `...`, start → the profile again (the proxy follows its service's owner). Started with tuned-ppd stopped → it showed anyway: **D-Bus activation**, the bus starts the service for the request. `gaming_hub --end` with no hub → silent, exit 0 (the proxy stays `NULL`, the unref is skipped).

## Switching (step 3, 22 Nov)

- **One toggle button per profile**, built from `Profiles` (so 2 profiles on a laptop without `performance` give 2 buttons): walked with a `GVariantIter` on the stack, `g_variant_iter_next_value` (each child owned, unref'd), `g_variant_lookup(dict, "Profile", "&s", &name)` (`&s` = borrowed). Joined in a row with the `linked` CSS class (spacing 0), grouped with `gtk_toggle_button_set_group` (only one pressed), the leader found with `gtk_widget_get_first_child`.
- **Each button remembers its profile:** `g_object_set_data_full(button, HUB_PPD_KEY, g_strdup(name), g_free)`, freed with the button; read back with `g_object_get_data` (borrowed).
- **Showing:** `hub_power_select` walks the row (`get_first_child` / `get_next_sibling`) and sets each button to `g_strcmp0(name, active) == 0`, with its `toggled` handler blocked (no echo: GNOME's change would otherwise be sent straight back). All released when the profile is unknown.
- **Writing:** `g_dbus_proxy_call(proxy, "org.freedesktop.DBus.Properties.Set", g_variant_new("(ssv)", iface, "ActiveProfile", g_variant_new_string(name)), …)`. A method name with dots calls that interface's method. The `(ssv)` is a **floating** reference: the call takes it over, never unref it. `g_dbus_proxy_set_cached_property` would only change the local cache, tuned-ppd would never hear it. The hub doesn't update its own display after the call: tuned-ppd's `PropertiesChanged` does (one source of truth).
- **`toggled` fires twice per click** in a group (the pressed button and the released one): only the active one sends.
- **Errors:** `hub_power_set_done` (`g_dbus_proxy_call_finish`: the empty reply `()` is still owned and unref'd); on failure the remote error name is cut with `g_dbus_error_strip_remote_error`, the message printed, and `hub_power_show` puts the buttons back to the real profile. Tested by sending `"nope"`: `power: Invalid profile 'nope'`, the button snapped back.
- **Known limit:** `Profiles` is read once, when the proxy is ready. On a machine where the service appears later, the buttons would not appear (v2 portability).

## Battery alerts (step 4, 22 Nov)

Planned as a UPower `OnBattery` proxy; done in `sources/computer/battery.c` instead, because the once-a-second tick already reads the charger (`online` in the Mains folder) and the level (`capacity`) from `/sys`. Decided with the user:

- **Unplug** (`hub_battery_unplug`): alert once on plugged → unplugged, **during a session only** (`hub_timer_running`: playing, break, between games; not Paused). The last state seen is remembered in `was_plugged` every tick, alert or not; a failed read changes nothing. Starts at `TRUE`, so starting on battery is not an unplug.
- **Low battery** (`hub_battery_low`, called after `hub_battery_unplug` so `was_plugged` is this tick's state): one "Battery at N %" alert at **≤ 25 %** (`HUB_BAT_LOW`) during a session, no time estimate (it depends too much on the game); `low_warned` resets when plugged in. GNOME warns on its own at its low level (around 10 %), so 25 % comes earlier, not as a duplicate.
- Both use `hub_alert`, so they follow the session's pop-up and sound checkboxes. Tested with `HUB_BAT_LOW` at 100: silent on the plan page, one alert at session start, no repeat, both alerts again after replug + unplug.
