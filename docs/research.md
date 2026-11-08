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

*To do (Part B): options, where the log goes, whether the file grows during a game.*
