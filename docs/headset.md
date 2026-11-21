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

## Connect to the sound server (milestone 5, step 2, 21 Nov)

**Build.** The Makefile asks `pkg-config` for both packages in one call, `pkg-config --cflags gtk4 libpulse-mainloop-glib` (and `--libs`): it merges their flags and drops what both share (GLib). libpulse adds `-D_REENTRANT -pthread` (it is thread-safe) and links `-lpulse-mainloop-glib -lpulse`; its headers are in `/usr/include/pulse/`, already on the compiler's path, so they are included as `<pulse/...>`. The variables are named `PKG_CFLAGS` / `PKG_LIBS` since they no longer hold only GTK. New area: `sources/headset/`, `includes/headset.h`, the `HEADSET` list.

**Asynchronous, like GTK.** libpulse never waits for the server: a request returns at once, and the answer comes later in a callback, called from the main loop. So the window never blocks.

| Piece | Created with | Owner |
| --- | --- | --- |
| GLib adapter (`pa_glib_mainloop`) | `pa_glib_mainloop_new(NULL)`: `NULL` = GLib's default context, the one GTK runs | the hub, `pa_glib_mainloop_free`, **last** |
| API table (`pa_mainloop_api *`) | `pa_glib_mainloop_get_api(adapter)`: how libpulse adds timers and watches sockets | borrowed from the adapter, never freed |
| Context (`pa_context`) | `pa_context_new(api, "Gaming Hub")`: the one connection; the name is what `pactl list clients` shows | the hub, reference-counted, `pa_context_unref` |
| State callback | `pa_context_set_state_callback(context, hub_pulse_state, hub)`: `hub` comes back as `user_data` | — |

`pa_context_connect(context, NULL, PA_CONTEXT_NOFLAGS, NULL)` (`NULL` = the default server, PipeWire) starts connecting and returns at once; negative = failed right away (`pa_strerror(pa_context_errno(context))` says why). From then on the state callback is called at each change; `pa_context_get_state` gives `UNCONNECTED` (0), `CONNECTING` (1), `AUTHORIZING` (2), `SETTING_NAME` (3), **`READY` (4)**, **`FAILED` (5)**, `TERMINATED` (6). Ready = sinks and sources can be asked for (step 3); failed and terminated never come back by themselves.

`hub_pulse_connect` runs once, in `hub_activate` after the early return (like the tick). It stores the adapter and the context in `t_hub`'s new `t_headset` and stops at the first `NULL`, without freeing: `hub_free_hub` frees whatever isn't `NULL`. It returns nothing: if the sound server can't be reached, the hub still runs, only without headset control.

**Freeing, in reverse order** (found the hard way, 21 Nov):

1. `pa_context_disconnect`: closes the connection on purpose; the state callback hears it one last time (state 6).
2. `pa_context_unref`: drops the hub's reference.
3. `pa_glib_mainloop_free`: the adapter, **last**.

- **Adapter first = abort.** The first version freed the adapter, then unref'd the context: `ERROR:…glib-mainloop.c:256:glib_io_free: assertion failed: (!e->dead)`, SIGABRT at quit. A connected context has its socket registered with the adapter; freeing the adapter marks every registration dead, and the context then tries to remove its socket from an adapter that's gone.
- **No `NULL`.** Unlike `g_free`, libpulse's free functions assert on `NULL`. Each call has its own check: `gaming_hub --end` with no hub running builds nothing, so both pointers stay `NULL`.
- Without the disconnect it also worked (the last unref closes the connection silently), but the state callback never heard it.

**Tested (21 Nov):**

| Run | Output |
| --- | --- |
| `./gaming_hub`, close the window | `pulse: state 1`, `2`, `3`, `4`, at quit `6`; clean exit |
| `./gaming_hub --end`, no hub running | nothing printed, exit code 0 |
| `env PULSE_SERVER=unix:/tmp/nothing-here ./gaming_hub` | `state 1`, `state 5`, then `pulse: Connection refused`; the window works, clean exit |

The failure test shows that **the state callback can run inside `pa_context_connect`**, before it returns: state 5 is printed before the connect's own error line. So the callback must never rely on something set up after the connect call. After a failure, the disconnect at quit prints nothing (it only announces "terminated" for a connection that was still good).
