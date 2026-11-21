# Headset (milestone 5)

Volume and mute of the headset's output and mic, through libpulse (PipeWire speaks the PulseAudio API through `pipewire-pulse`), kept in sync when they change elsewhere (GNOME's menu, the keyboard's volume keys). The headset's battery comes later, from BlueZ over D-Bus (milestone 6, see [`research.md`](research.md)). Back to the [README](../README.md).

## The steps

| Step | What | Estimate |
| --- | --- | --- |
| 1. Look by hand | Dev package, how PipeWire sees the headset (`pactl`), the events | ~0.5 h |
| 2. Connect to the sound server | `sources/headset/`, libpulse in GTK's main loop (`pa_glib_mainloop`), the context and its states, cleanup | 3–4 h |
| 3. Find the headset, read it | Sink and source found by description, volume % and mute shown, "no headset" | 2 h |
| 4. Controls | Slider and mute switch for output and mic | 2 h |
| 5. Stay in sync | `pa_context_subscribe`, headset off / on, no feedback loop | 2 h |
| 6. Wrap-up | CPU check, norminette, docs | ~0.5 h |

## Look by hand (milestone 5, step 1, 21 Nov)

**The package.** Building against libpulse needs its development package (headers and `pkg-config` files; the libraries themselves are already there for PipeWire): `sudo dnf install pulseaudio-libs-devel` on Fedora (17.0 on Fedora 44), `libpulse-dev` on Debian / Ubuntu. The hub links to **`libpulse-mainloop-glib`**, the part that runs libpulse inside GLib's main loop; it pulls in `libpulse` itself. Check: `pkg-config --modversion libpulse-mainloop-glib`.

**Words.** A **card** is a physical device (the laptop's sound chip, the headset). In its active **profile** it offers **sinks** (outputs) and **sources** (inputs). Every sink also has a **monitor** source (`<sink name>.monitor`, "Monitor of …"): a copy of what plays on it, for recording; not a mic. A **sink-input** is one program's stream playing into a sink; a **client** is a program connected to the server.

**How PipeWire shows the Xbox Wireless Headset** (`pactl list sinks`, `pactl list sources`, `pactl list cards`, headset on, music playing mode):

| What | Index (changes) | Name (changes with the address) | Description | Channels |
| --- | --- | --- | --- | --- |
| Card | `#126` | `bluez_card.54_4C_8A_E1_83_E0` | — | — |
| Output (sink) | `#130` | `bluez_output.54_4C_8A_E1_83_E0.1` | `Xbox Wireless Headset` | 2 (front-left, front-right) |
| Mic (source) | `#128` | `bluez_input.54:4C:8A:E1:83:E0` | `Xbox Wireless Headset` | 1 (mono) |
| Monitor (source) | `#130` | `bluez_output.54_4C_8A_E1_83_E0.1.monitor` | `Monitor of Xbox Wireless Headset` | 2 |

- **Found by description**, not by index or name: the index is new after every reconnect (`#130` → `#153`), the name holds the Bluetooth address (and the mic writes it with `:`, the output with `_`). Same rule as the sensors in milestone 2.
- **Sink and source share the description**: the hub looks for "the sink called X" and "the source called X" in libpulse's two separate lists.
- **Compare the whole description** (equal, not "contains"), so "Monitor of Xbox Wireless Headset" never matches.
- **Volume:** one value per channel; `65536` = 100 % = libpulse's `PA_VOLUME_NORM`. The output has 2 channels, the mic 1: the code must not assume a channel count.
- The laptop's own sinks (three HDMI outputs, the speaker) and mics are ignored.

**The mic is there in music mode.** The card's active profile was `a2dp-sink` (good sound, no mic in Bluetooth terms), yet the mic source was listed. PipeWire keeps a virtual mic there and switches the card to the hands-free profile (phone-call sound) only when a program actually **records** from it. Reading or setting the mic's volume and mute records nothing, so the hub doesn't spoil the game's sound. This corrects the expectation in `research.md` (mic only in hands-free).

**Events** (`pactl subscribe`, one line per event; this is what step 5 subscribes to):

| Done by hand | Events seen |
| --- | --- |
| Dragging GNOME's volume slider | a stream of `'change' on sink #130`, one per small step |
| GNOME's volume "pop" sound | `'new'` client + sink-input, a few `'change'`, then `'remove'` on the sink-input |
| Headset off | `'remove'` on sink `#130`, its monitor `#130`, the mic `#128`, the card `#126`; `'change'` on **server** |
| Headset on | `'new'` card `#149`, **mic first** (source `#151`), then sink `#153` and its monitor `#153`; `'change'` on server |

- An event says **what** changed and its index, never the new value: the hub asks for that device's info again.
- **New numbers** after a reconnect: on `'new'` the hub reads the device and checks its description; on `'remove'` it compares the index with the one it keeps.
- The mic can arrive **before** the output: the two are found independently.
- `server #4294967295` is `0xFFFFFFFF`, libpulse's `PA_INVALID_INDEX` (the server has no index): the server's defaults changed (the default output moved to the headset and back).
- The hub subscribes only to sinks and sources (and maybe the server), so sink-inputs and clients never reach it.
