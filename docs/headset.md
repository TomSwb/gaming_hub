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

## Find the headset, read it (milestone 5, step 3, 21 Nov)

**How a list arrives.** `pa_context_get_sink_info_list(context, hub_sink_list, hub)` (and `pa_context_get_source_info_list` for the mic) is sent from the state callback once the state is `PA_CONTEXT_READY`. It returns a `pa_operation *` at once, a handle on the request in progress, owned by the hub: not needed here, so it is dropped right away with `pa_operation_unref` (the request still completes). It is `NULL` if the request couldn't be sent, and `pa_operation_unref` asserts on `NULL`, hence the check.

The list callback is then called **many times**:

| `eol` | `i` | Meaning |
| --- | --- | --- |
| `0` | one device's info | once per sink (or source) |
| `> 0` | `NULL` | end of the list: everything has been seen ("no headset" is decided here) |
| `< 0` | `NULL` | the request failed |

**`i` is borrowed for that one call**: libpulse frees it when the callback returns. The hub copies what it keeps (the index) and never stores the pointer.

**Matching.** `g_strcmp0(i->description, HUB_HEADSET_NAME) == 0` (`HUB_HEADSET_NAME` in `defaults.h`): `g_strcmp0` is `strcmp` that accepts `NULL`. Equal, not "contains", so "Monitor of Xbox Wireless Headset" never matches; only one mic is found. The found index goes into `t_headset` (`sink_index`, `source_index`), which start at **`PA_INVALID_INDEX`** (`0xFFFFFFFF`, "no index"; `0` could be a real one). At the end of the list, an index still at `PA_INVALID_INDEX` means no headset.

Bugs on the way (17:42): the name in quotes (`"HUB_HEADSET_NAME"` is that 16-letter text, the preprocessor doesn't replace inside quotes) and `if (g_strcmp0(…))` without `== 0` (true when **different**): every sink was "found", and the index kept was the last sink's. The clue was the count: 5 "found" lines for 5 sinks.

**Volume as a %:** `hub_volume_percent` (`volume.c`, shared by output and mic): `pa_cvolume_avg` averages the channels (2 for the output, 1 for the mic), then `(average × 100 + PA_VOLUME_NORM / 2) / PA_VOLUME_NORM`; adding half the divisor rounds to the nearest instead of down. No clamping: GNOME's over-amplification goes above 100 %.

**Mute is separate from the volume:** muted at 61 % still reads 61 %, only `mute` changes (that's why unmuting returns to the same level). The labels show both: `Volume: 61%` / `Volume: 61% (muted)`. GNOME's slider at the bottom gives 0 % and mutes as well.

**On screen.** `hub_build_headset` (`headset_build.c`) builds a "Headset" section between the timer pages and the stats: a title and two labels (output, mic), starting at `...`. `hub_show_sink` / `hub_show_source` make the text (`NULL` = nothing found) and set it once. `hub_activate` builds the window **before** `hub_pulse_connect`, so the labels exist before any libpulse callback can touch them.

**The mic and GNOME.** In music mode (A2DP, "Portable" in GNOME) GNOME Settings shows **"No Input Devices"**: it doesn't list PipeWire's virtual headset mic. libpulse does, so the hub finds it, and step 4's controls will be the only easy way to set or mute it in that mode. By hand: `pactl set-source-mute bluez_input.54:4C:8A:E1:83:E0 toggle`, `pactl set-source-volume … 80%`. GNOME lists the headset twice as an output, "Portable" (A2DP, good sound) and "Handsfree" (HFP, mic on, phone-call sound for everything); choosing one switches the card's profile (also `pactl set-card-profile bluez_card.<address> a2dp-sink`). Decided 21 Nov: stay on Portable for games. After a switch the headset's sink and source get new indexes: the hub sees that from step 5.

**Tested (21 Nov):** headset on (`Volume: 70%`, then 61 %, muted, 0 % muted), headset off (no device), mic found once (the monitor skipped) and its mute toggled with `pactl`.

## Controls (milestone 5, step 4, 21 Nov)

**The layout.** The headset section has the "Headset" title centred, then two columns side by side, output and mic, each with its label, a slider (GtkScale, 0–100, step 1) and a **Mute** toggle button (`hub_build_sliders`, `hub_build_mute`, `hub_headset_build_box` in `headset_build.c`). The sliders and buttons start **greyed out** (`gtk_widget_set_sensitive(…, FALSE)`), so before libpulse answers, or if the server can't be reached, nothing can be moved while the labels say "off". `hub_sink_set_buttons` / `hub_source_set_buttons` put the device's values on its controls and make them sensitive; `hub_show_*` greys them out again when the device is gone.

**Slider → volume** (`hub_sink_slider_moved`, `hub_source_slider_moved`, on `"value-changed"`):

1. No device (index still `PA_INVALID_INDEX`) → return.
2. `gtk_range_get_value` → the % (a `double`).
3. The new loudest value: `(% × PA_VOLUME_NORM + 50) / 100` (adding half of 100 rounds to the nearest), the reverse of `hub_volume_percent`.
4. **The balance is kept:** the hub stores a **copy** of the device's `pa_cvolume` in `t_headset` (`sink_vol`, `src_vol`: struct fields, not pointers, copied with `=` from `i->volume` when the device is shown, so they stay valid after libpulse frees `i`; `pa_cvolume_init` in the init = 0 channels, "empty"). The callback copies it into a local `pa_cvolume` and calls `pa_cvolume_scale(&local, new_max)`: the loudest channel becomes `new_max` and the others keep their ratio. A fresh volume with every channel set equal would wipe the left/right balance.
5. `pa_context_set_sink_volume_by_index(context, sink_index, &local, NULL, NULL)` (`…_source_…` with `source_index` for the mic): no success callback, the answer isn't needed. The `pa_operation *` is unref'd behind a `NULL` check.

**Order matters in `hub_*_set_buttons`:** the volume is copied **before** `gtk_range_set_value`, because `set_value` emits `"value-changed"` at once: the callback runs during that line and would otherwise scale the empty `pa_cvolume`.

**Mute** (`hub_sink_mute_toggled`, `hub_source_mute_toggled`, on `"toggled"`): `pa_context_set_sink_mute_by_index(context, sink_index, gtk_toggle_button_get_active(self), NULL, NULL)` (and `…_source_…`). The button's state (`TRUE`/`FALSE` = 1/0) is the `mute` argument itself, no `if`. Shown with `gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(button), i->mute)`. A GtkToggleButton rather than a GtkSwitch: its `"toggled"` callback has the simple `(self, user_data)` shape.

**Echo, for step 5:** showing a value (`gtk_range_set_value`, `gtk_toggle_button_set_active`) emits the same signal as the user moving it, so each show sends the same value back to the server once. Harmless here; with events it would loop.

**Bugs on the way (21 Nov):**

| Symptom | Cause | Fix |
| --- | --- | --- |
| SIGSEGV right after `pulse: state 4` | `hub_volume_percent(&i->volume)` above `if (i == NULL)`: a device not found → `&i->volume` near address 0 | read the volume only in the branch where `i` is valid |
| sliders never followed the headset | values and sensitivity set once at **build**, before libpulse answered; and `if (source_index)` is true for `PA_INVALID_INDEX` (`0xFFFFFFFF`, not 0) | set them in `show`, when the answer arrives; "found" is `!= PA_INVALID_INDEX` |
| "the mic works" but the mic never changed | copy-paste: the mic handler connected to the output slider, and the mic callback checked `sink_index` and called `…_set_sink_volume…` with `sink_index` (each output move set the output twice, the second time with the mic's mono volume) | the four places changed to the source; checked with `pactl get-source-volume` |
| a mute that only ever mutes | `gtk_toggle_button_get_active(self);` alone on a line (answer thrown away), then a fixed `1` | the call's result as the `mute` argument |

**Tested (21 Nov):** output slider → the sound and GNOME's slider follow; mic slider → `pactl get-source-volume bluez_input.54:4C:8A:E1:83:E0` follows, the output doesn't move; output Mute → silence, GNOME shows muted; mic Mute → `pactl get-source-mute …` says `Mute: yes`. The labels don't follow the controls yet: step 5.
