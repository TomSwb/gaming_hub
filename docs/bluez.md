# Headset line: BlueZ and the default output (milestone 6, step 5)

Under the "Headset" title, one line says what I'm listening on: a Bluetooth headset's name and battery (`Xbox Wireless Headset · 50 %`), or the output's short name (`Speaker`, `HDMI / DisplayPort 1 Output`). Any headset, not only the Xbox. Back to the [README](../README.md).

> **Later the same evening** the line changed: what I'm listening on became a row of buttons, and the line now only shows the battery (`Battery: 50 %`) beside the "Sound" title, hidden when the output isn't Bluetooth. The headset's name is no longer read from BlueZ (`hub_bluez_with_battery` merged into `hub_bluez_text`) and the output's short name is no longer stored (`hub_set_output` became `hub_set_bt_address`). See [`sound.md`](sound.md). This page describes steps 5a–5b as they were built.

## The rule (decided 22 Nov)

The line follows **PipeWire's default output**, the one I'm hearing:

1. libpulse gives the default sink; its property `api.bluez5.address` is the Bluetooth address (`54:4C:8A:E1:83:E0`), or missing for the laptop's own outputs;
2. BlueZ's device with that `Device1.Address` gives the name, `Connected` and, through `Battery1.Percentage`, the battery.

No choosing, nothing to kick out: switch the output in GNOME and the line follows. Other ways considered: one line per connected headset (both headsets were connected at once, which one is in use?), or preferring the Xbox by name.

## Look by hand (22 Nov)

**BlueZ's objects** (`busctl tree org.bluez`): the adapter `/org/bluez/hci0`, one `dev_<address>` per paired device (4 here: JBL Live 770NC, Xbox Wireless Headset, MX Master 3S, LIFT), and under a connected headset `fd0` (the open audio stream, `MediaTransport1`) and `sep1` (the agreed audio format, `MediaEndpoint1`). A paired device stays listed while it's off.

**Name, icon, connected** for each (`busctl get-property org.bluez <path> org.bluez.Device1 Name Icon Connected`): JBL `audio-headset`, Xbox **`audio-card`**, the mice `input-mouse`. A rule on the icon would need "starts with `audio-`", since the Xbox doesn't call itself a headset. Matching by name only would need the exact name (two headsets, two mice with batteries).

**Off / on** (`gdbus monitor --system --dest org.bluez`):
- off: `Device1` `Connected: false` on the device's path, then on `/` `InterfacesRemoved` for `fd0`, `sep1`, and the device's **`Battery1`**;
- on: `Connected: true`, `sep1` and `fd0` come back, and last, on `/`, **`InterfacesAdded`** for the device with `{'org.bluez.Battery1': {'Percentage': <byte 0x32>}}` (50 %). The first value arrives inside `InterfacesAdded`, later drops as `PropertiesChanged` on `Battery1`.

**PipeWire's side** (`pactl get-default-sink`, `pactl list sinks`): the default sink is `bluez_output.54_4C_8A_E1_83_E0.1` (Xbox), `bluez_output.00_A4_1C_DB_6F_2B.1` (JBL) or `alsa_output.…HiFi__Speaker__sink`. The Bluetooth sinks carry `api.bluez5.address = "54:4C:8A:E1:83:E0"`, in BlueZ's own format; the laptop's outputs have `device.bus = "pci"` and no address. Their descriptions are long (`500 Series Chipset Family … HDMI / DisplayPort 1 Output (Stereo)`), but `device.profile.description` is short: `Speaker`, `HDMI / DisplayPort 1 Output`. Bluetooth sinks don't have it; their description is already short (`Xbox Wireless Headset`).

## BlueZ (step 5a, 22 Nov, ~2.1 h with the decisions)

- **`GDBusObjectManagerClient`** on `org.bluez`, object path `/` (`g_dbus_object_manager_client_new_for_bus`, async, plain `GDBusProxy`s: the three proxy-type arguments `NULL`). It calls `GetManagedObjects`, keeps one proxy per interface of every object, and follows `InterfacesAdded` / `InterfacesRemoved` / `PropertiesChanged` itself. Owned by `t_headset`, unref'd at quit.
- **Finding** (`hub_bluez_find`): `g_dbus_object_manager_get_objects` returns a `GList` you own twice (the list and a reference on every object): walked with a second pointer, the head kept for `g_list_free_full(head, g_object_unref)`. The match is `g_object_ref`'d before the free and handed to the caller. `hub_bluez_has_address` asks one object for `Device1.Address`.
- **One helper for every read** (`hub_bluez_prop`): `g_dbus_object_get_interface` (owned, `NULL` if the object lacks it) → `g_dbus_proxy_get_cached_property` → interface unref'd → the value returned, owned by the caller.
- **The text** (`hub_bluez_text`, `hub_bluez_with_battery`): `Connected` (`b`, `g_variant_get_boolean`) false → nothing; else `Name`, plus `Percentage` (`y`, `g_variant_get_byte`, printed with `%u`) → `Name · N %`, or `Name · ...` while `Battery1` hasn't arrived.
- **Live:** `interface-added`, `interface-removed` and `interface-proxy-properties-changed` on the manager; the cache is already updated, so they just redraw. Connected with **`g_signal_connect_swapped`** straight to `hub_bluez_show`: the data (`hub`) comes first and the signal's other arguments are ignored (a GLib idiom, safe through `G_CALLBACK`). First written as two handlers with the signals' full shapes (`bluez_events.c`); `interface-proxy-properties-changed` has 6 parameters, over the norm's 4, so they were replaced the same day. They fire for every BlueZ device (a mouse's battery…); a redraw searches ~10 objects, negligible.
- **Unref the right family:** `g_variant_unref` for `GVariant`s, `g_object_unref` for GObjects (interfaces, objects, proxies); `g_object_unref` takes a `gpointer`, so the compiler doesn't catch a mix-up.

## Default output (step 5b, 22 Nov, ~0.7 h)

- **`sources/headset/default.c`:** `hub_pulse_default` (`pa_context_get_server_info`'s callback, called once, no `eol`; `default_sink_name` may be `NULL`) asks for that sink with `pa_context_get_sink_info_by_name`; `hub_default_sink` reads `api.bluez5.address` and the short name with `pa_proplist_gets` (borrowed, `NULL` if missing; the short name falls back to `i->description`); `hub_set_output` frees the old copies, keeps new ones (`bt_add`, `outp_name`) and redraws.
- **Live:** `PA_SUBSCRIPTION_MASK_SERVER` added to the subscription; a `SERVER` event asks for the server info again. Switching the output in GNOME, or a headset turning off (PipeWire falls back to another output), updates the line.
- **The label** (`hub_bluez_show`): the Bluetooth text if the default output is a connected BlueZ device, else the output's name, else `...`. Before BlueZ answers, a Bluetooth output still shows its name from PipeWire. libpulse and BlueZ start in parallel; either order ends on the right text.
- **Tested** with the hub running: Xbox → `Xbox Wireless Headset · N %`, speakers → `Speaker`, JBL → `JBL Live 770NC · N %`, Xbox off → the fallback output. HDMI: to test with the projector.
- **Next** (done the same evening, [`sound.md`](sound.md)): the sliders follow the default output and input, and the output is chosen from the hub.
