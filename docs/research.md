# Research notes (milestone 0)

What I checked by hand before coding, and what the code will need. Tested on 8 Nov 2026, Fedora, ThinkPad T15 Gen 2i.

## 1. Headset battery

**Headset:** Xbox Wireless Headset, connected over **Bluetooth** (no USB cable).

### Where the battery is (and isn't)

| Source | Battery? | Command used |
| --- | --- | --- |
| BlueZ (Bluetooth stack) | ✅ yes, e.g. 50 % | `bluetoothctl info <address>` |
| UPower | ❌ headset not listed | `upower -e` |
| PipeWire | ❌ no battery, but the sound card is there | `pactl list cards` |
| Kernel (`/sys/class/power_supply`) | ❌ only `BAT0` and `AC` | `ls /sys/class/power_supply` |

→ The hub reads the battery **from BlueZ over D-Bus** (system bus).

### D-Bus: the 4 things needed to read a value

| Part | Value for the headset battery |
| --- | --- |
| Service (who answers) | `org.bluez` |
| Object path (which object) | `/org/bluez/hci0/dev_54_4C_8A_E1_83_E0` (adapter + address, `:` → `_`) |
| Interface (group of properties) | `org.bluez.Battery1` |
| Property | `Percentage` |

Bus: the **system** bus (not the session bus).

Same object, other useful property: `org.bluez.Device1` → `Connected` (`b`, true/false), `Name` (`s`).

Check by hand:

```fish
busctl get-property org.bluez /org/bluez/hci0/dev_54_4C_8A_E1_83_E0 org.bluez.Battery1 Percentage
# → y 50
busctl introspect org.bluez /org/bluez/hci0/dev_54_4C_8A_E1_83_E0
```

### D-Bus type codes seen

| Code | Type | Read in C (GVariant) |
| --- | --- | --- |
| `y` | byte, 0–255 | `g_variant_get_byte()` |
| `b` | boolean | `g_variant_get_boolean()` |
| `s` | string | `g_variant_get_string()` (no copy, don't free) |
| `o` | object path | `g_variant_get_string()` |
| `as` | array of strings | — |
| `a{sv}` | dictionary string → any value | the shape of `PropertiesChanged` |

### Updates: signals, not polling

Every property is flagged `emits-change`, so BlueZ sends `org.freedesktop.DBus.Properties.PropertiesChanged` when a value changes. The hub subscribes and does nothing in between (no fps cost).

Seen with `gdbus monitor --system --dest org.bluez --object-path <path>` while turning the headset off and on:

1. **Off:** `Device1.Disconnected` signal, then `PropertiesChanged` on `org.bluez.Device1` with `Connected: false`.
2. **On:** `PropertiesChanged` with `Connected: true`, then **a moment later, separately**, `PropertiesChanged` on `org.bluez.Battery1` with `Percentage`.

`PropertiesChanged` carries 3 things: the interface name (`s`), the changed values (`a{sv}`), the invalidated property names (`as`).

### Design rules for the code

- **Don't hard-code `hci0` or the address.** Find the device by name ("Xbox Wireless Headset") among BlueZ's objects (the ObjectManager at `/`, method `GetManagedObjects`), like finding a hwmon sensor by name, not by number.
- **Battery only while connected.** When the headset is off, show "off", not an old number.
- **The battery arrives after the connection.** Show "—" until the first `Percentage` arrives.
- **`Battery1` comes and goes.** BlueZ announces it with `InterfacesAdded` / `InterfacesRemoved` on the root object `/`, not on the device path. Listen there too.
- **Bluetooth not running / headset never paired:** the hub must still start, the panel just shows no headset.

### Audio (milestone 5)

- PipeWire sees the headset as a sound card named **"Xbox Wireless Headset"** (the card number changes, don't rely on it).
- Over Bluetooth, using the mic switches the headset to the hands-free profile (phone-call sound quality). Not an issue for solo games without voice chat.

### Docs

- GDBus overview and `GDBusProxy`: <https://docs.gtk.org/gio/class.DBusProxy.html>
- Watching signals: `g_dbus_connection_signal_subscribe()`, <https://docs.gtk.org/gio/method.DBusConnection.signal_subscribe.html>
- GVariant format strings: <https://docs.gtk.org/glib/gvariant-format-strings.html>
- BlueZ API (`Device1`, `Battery1`): <https://github.com/bluez/bluez/tree/master/doc> (`org.bluez.Device.rst`, `org.bluez.Battery.rst`)
- `man busctl`, `man gdbus`

## 2. MangoHud logging

**Version:** MangoHud 0.8.2 (Fedora 44, 64-bit and 32-bit packages). Reference for every option: `/usr/share/doc/mangohud/MangoHud.conf.example`.
**My config** is a symlink: `~/.config/MangoHud/MangoHud.conf` → `~/dotfiles/mangohud/.config/MangoHud/MangoHud.conf` (edit and commit it in the dotfiles repo).

Tested with `mangohud vkcube` (Vulkan test cube, capped at 30 fps by my config).

### Options

| Option | What it does | For the hub |
| --- | --- | --- |
| `output_folder` | Where the logs go. **Required**, no logging without it | `/home/tomswb/mangologs` |
| `log_interval` | Milliseconds between rows (`0` = MangoHud's default) | `1000` = one row per second |
| `autostart_log` | Start logging X seconds after launch, no key | `3` |
| `log_duration` | Stop logging after X seconds | ❌ **never use** (crashes, see below) |
| `toggle_logging` | Key to start/stop logging (default `Shift_L+F2`) | ❌ **never press** during a game (crashes) |

### The log file

- **Name:** `<program>_<YYYY-MM-DD>_<HH-MM-SS>.csv` in `output_folder`. The hub takes the **newest** file.
- **Written live:** the line count grows about once per second while the game runs (`watch -n 2 'wc -l ~/mangologs/*.csv'`). → **Reading fps live is possible** (milestone 9): remember how far you read, read the new lines each second.
- **3 header lines** before the data:
  1. `os,cpu,gpu,ram,kernel,driver,cpuscheduler`
  2. their values (Fedora 44, i5-1135G7, Iris Xe, …)
  3. the column names:
  `fps,frametime,cpu_load,cpu_power,gpu_load,cpu_temp,gpu_temp,gpu_core_clock,gpu_mem_clock,gpu_vram_used,gpu_power,ram_used,swap_used,process_rss,cpu_mhz,elapsed`
- **Find columns by name** from line 3, not by position (the order can change between versions).
- **Units:** `fps` per second, `frametime` in ms, `ram_used` in GB, `elapsed` in **nanoseconds** since the start (`1001782410` ≈ 1.0 s).
- **Always 0 on this laptop:** `gpu_load`, `gpu_temp`, `gpu_core_clock`, `gpu_mem_clock`, `cpu_power`. The hub reads the GPU from `/sys` itself (milestone 2). Only `fps` and `frametime` are worth taking from MangoHud.
- **CPU power can't be read** without root (Intel RAPL in `/sys/class/powercap` is root-only). MangoHud prints `Failed to initialize CPU power data` for that reason; harmless. The hub can't show CPU watts either.

### The summary file

- `<same name>_summary.csv`: one header line + one line of averages and peaks (average fps, 1 % min, …).
- **Written only when logging stops**, so not at all with autostart + quit (see below).
- **No `\n` at the end of the last line** (`cat` glues two summaries together). Any parser must accept a last line without a newline (like `get_next_line`).
- **Not reliable:** one run gave average fps `0.0` and frame time `60003` ms while the cube ran at 30 fps.
- → **The hub computes average fps itself** from the per-second rows. It doesn't use the summary.

### ⚠️ Crash when logging stops (MangoHud 0.8.2)

Every time logging **stops while the game runs**, MangoHud writes the summary and then the game is killed (`SIGABRT`):

```
imgui.cpp:10783: bool ImGui::ItemAdd(...): Assertion `id != window->ID && "Cannot have an empty ID at the root of a window..."' failed.
```

| Test | Result |
| --- | --- |
| `log_duration=60` (2 runs) | ❌ crash at 60 s |
| `toggle_logging` key (Shift_L+F2) | ❌ crash |
| Same key with a minimal config (`MANGOHUD_CONFIGFILE=/tmp/mh-test.conf`) | ❌ crash → not caused by my HUD settings |
| `autostart_log`, never stopped, window closed | ✅ clean exit, CSV complete, no summary |

It is a bug in MangoHud 0.8.2 on this system, not in my config. Fedora had no update on 8 Nov (`dnf list --upgrades mangohud`); upstream 0.8.3 (April 2026) lists crash fixes. Re-test when Fedora ships it.

### Rules for the hub and the config

In my config since 8 Nov (dotfiles `8aa59ec`): `output_folder=/home/tomswb/mangologs`, `log_interval=1000`, `autostart_log=3` + a warning comment. Other keys in my config: Shift_L+F1 cycles the fps cap (30 / 60 / none; the log shows the real fps), Shift_R+F12 hides/shows the HUD (logging should go on underneath, to test with a real game).


- Config: `output_folder`, `log_interval=1000`, `autostart_log=3`. **No `log_duration`.** Never press the logging key in a game.
- Logging runs for the whole game and ends when the game quits. No summary → the hub doesn't need one.
- Logs pile up in `~/mangologs` (about 4.5 KB per minute → ~270 KB per hour). The hub, or I by hand, cleans old ones later.
- MangoHud only runs in games where I added it (Steam launch option `gamemoderun mangohud %command%`, Heroic's per-game switch). No MangoHud → no fps file → the fps panel shows "—".

### Docs

- MangoHud README (options, logging): <https://github.com/flightlessmango/MangoHud>
- `/usr/share/doc/mangohud/MangoHud.conf.example`
- Following a file that grows: `GFileMonitor` (<https://docs.gtk.org/gio/class.FileMonitor.html>) to see a new log appear; `GInputStream` / plain `read()` from the last offset for the new lines.
- `man watch`, `man wc`, `man tail` (`tail -f` is the idea)

## 3. Summary for the plan

| Question | Answer |
| --- | --- |
| Headset battery? | ✅ BlueZ over D-Bus (Bluetooth), signals, no polling |
| Live fps? | ✅ MangoHud CSV grows each second, read new lines |
| GPU stats from MangoHud? | ❌ all 0 on Iris Xe → read `/sys` |
| CPU watts? | ❌ root only |
| Safe logging setup? | autostart only, never stop it (0.8.2 crash) |
