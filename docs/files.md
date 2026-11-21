# Files

What each file does. Back to the [README](../README.md).

## Project files

* `sources/main.c`: creates the application (`ch.bitminded.GamingHub`) and the hub's state struct, runs GTK's main loop, builds the window on the first activation (a later launch only brings the existing window to the front), reads each launch's options in `hub_command_line` (`--end`: a game closed; `--start`: a game started), starts the once-a-second timer (`hub_tick`), frees the struct's strings when the app ends
* `sources/inits.c`: `hub_struct_init` sets every field to a known value: `hub_computer_init` (stats), `hub_setting_init` (the settings to their defaults), `hub_timer_init` (timer widgets to `NULL`, state) and `hub_headset_init` (pointers to `NULL`, indexes to `PA_INVALID_INDEX`)
* `sources/main_build.c`: the window's layout and shared layout helpers: `hub_build_box` (a stack of pages, the headset section, the stats), `hub_image_with_label` (a theme icon at 48 px with an optional wrapping label under it), `hub_labelled` (a left-aligned label in front of a widget)
* `sources/utils.c`: `hub_setting_hub` (loads the timer settings, then labels + finding the sensor folders, then the timer widgets, once), `hub_free_hub` (frees what the struct owns; the libpulse connection in reverse order: disconnect, unref, adapter last)
* `sources/computer/`: the live stats (milestone 2)
  * `sysfs.c`: shared readers for `/sys`: `hub_find_dir` (finds a device folder by the content of one of its files, e.g. hwmon `name` = `coretemp`), `hub_find_gpu` (the DRM card that has `gt_act_freq_mhz`), `hub_read_number` and `hub_read_text` (one value from a one-value file)
  * `ram.c`: reads `/proc/meminfo`, shows used / total RAM
  * `cpu.c`: reads `/proc/stat`, shows CPU % from two samples one second apart
  * `temp_fan.c`: CPU package temperature (`coretemp`) and fan speed (`thinkpad`) on one line
  * `gpu.c`: GPU frequency (actual / maximum), from `/sys/class/drm`
  * `battery.c`: battery % and status, with the charger as fallback
  * `stats_build.c`: `hub_build_stats` (the five stat labels in a box)
* `sources/timer/`: the timer and healthy-gaming features (milestone 3)
  * `settings.c`: reads `~/.config/gaming_hub/settings.ini` with GKeyFile: `hub_load_timer_settings` (called first in `hub_setting_hub`, so every widget sees the final values), `hub_key_file_error` (loads the file, reports a broken one), `hub_set_timer_settings` (the seven keys), `hub_read_setting` (one number, keeps the default on any problem), `hub_read_setting_boolean` (one `true` / `false`)
  * `session.c`: `hub_timer_manager` (all timer widgets, with `hub_create_time_spin_buttons` and `hub_timer_create_buttons`), `hub_update_timer` (once a second from `hub_tick`: time left, breaks, eye cue), `hub_update_eye` (shows and hides the eye cue)
  * `alert.c`: `hub_notify` (a desktop notification), `hub_alert` (notification and/or the bell, as ticked), `hub_wind_down_alerts` (the wind-down warning, once per session)
  * `show.c`: what puts values on screen: `hub_show_time` (seconds → `h:mm:ss` on a label), `hub_show_timer_page` (the page that matches the state), `hub_show_eye` (the eye-cue text with `eye_sec`)
  * `buttons.c`: the button callbacks: `hub_start_clicked`, `hub_pause_clicked` (Pause ↔ Resume), `hub_stop_clicked` (End session), `hub_done_clicked` (back from a break), `hub_read_plan` (reads the plan page at Start)
  * `timer_build.c`: the timer's pages: `hub_build_plan` (three columns: checkboxes, `hub_plan_box_middle` with the three fields, alarm + Start), `hub_build_session`, `hub_build_break`, `hub_build_transit` (the "Between Games" page)
  * `timer_utils.c`: `hub_timer_running` (the states in which the countdown runs: playing, break, between games)
* `sources/game/`: games coming and going (milestone 4, later the launcher of milestone 7, the library)
  * `game_events.c`: `hub_game_closed` (`--end`: a running session → between games) and `hub_game_started` (`--start`: between games → playing)
* `sources/headset/`: the headset's output and mic through libpulse (milestone 5)
  * `pulse.c`: `hub_pulse_connect` (the GLib adapter, the context, the connection to the sound server, once, from `hub_activate`), `hub_pulse_state` (the context's state callback), `hub_pulse_ready` (when ready: the sink and source lists, the event callback and the subscription)
  * `events.c`: `hub_pulse_event` (the server's events: masks the facility and the type, then dispatches), `hub_sink_event` / `hub_source_event` (headset removed → off; new device → asked for by index; change of the headset → read and shown again)
  * `sink.c`: `hub_sink_list` (called once per output, then at the end: finds the headset by description, keeps its index), `hub_show_sink` (its volume and mute on the output label, or "no device" and the controls greyed out), `hub_sink_set_buttons` (keeps a copy of its volume, puts it on the slider (not within 300 ms of a user move) and the Mute button, with their handlers blocked so nothing is sent back), `hub_sink_slider_moved` (the slider sets the volume, balance kept, mutes at 0, remembers when it moved), `hub_sink_mute_toggled` (the Mute button sets the mute)
  * `source.c`: the same five for the mic (`hub_source_…`)
  * `volume.c`: `hub_volume_percent` (a `pa_cvolume`, any number of channels, as a rounded %)
  * `headset_build.c`: `hub_build_headset` (the "Headset" section), `hub_build_sliders` and `hub_build_mute` (the controls, greyed out until the headset is found, with their signals), `hub_headset_build_box` (the title, then two columns: output and mic, each label + slider + Mute)
* `includes/gaming_hub.h`: the header every `.c` file includes; pulls in the others and holds the prototypes of `main.c`, `utils.c`, `inits.c` and `main_build.c`
* `includes/structs.h`: the structs: `t_hub` holds a `t_computer` (stat labels, sensor folders, CPU samples), a `t_settings` (timer values) a `t_timer` (timer widgets and state) and a `t_headset` (libpulse's GLib adapter and context, the headset's output and mic indexes, copies of their volumes, and their labels, sliders and Mute buttons), all by value, plus the window's `stack` and the application (`app`, borrowed: `main` owns it)
* `includes/defaults.h`: the values the hub uses when the settings file doesn't set them (`HUB_DEF_…`)
* `includes/computer.h`, `includes/timer.h`, `includes/game.h`, `includes/headset.h`: prototypes of `sources/computer/`, `sources/timer/`, `sources/game/` and `sources/headset/`
* `Makefile`: builds the hub and libft, with GTK4's and libpulse's flags from `pkg-config`; also writes `compile_flags.txt` for the editor; `make install` / `make uninstall` (program in `~/.local/bin`, `.desktop` file in `~/.local/share/applications`)
* `assets/historic/`: screenshots of the hub at each stage (`first_working_prototype.png`, milestone 2; `timer_done/`, milestone 3: plan, playing, eye cue, break)
* `docs/research.md`: what I tested by hand before coding (headset battery over D-Bus, MangoHud logging) and the rules the code must follow
* `docs/files.md`, `docs/basics.md`, `docs/live-stats.md`, `docs/timer.md`, `docs/auto-start.md`, `docs/headset.md`: this file and how each part of the hub works, moved out of the README on 19 Nov 2026

## Also included

* `libft/`: my 42 libft (with `ft_printf` and `get_next_line`)
