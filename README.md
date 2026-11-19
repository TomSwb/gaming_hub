# Gaming_Hub - a program by TomSwb

## Description

#### Goal:

A small GTK4 window for my laptop screen while I game on the TV or projector. It opens when a game starts, shows what I care about during a session (system stats, headset, break timer, power), keeps a log of my sessions, and acts as the break reminder of my Healthy Gaming Charter.

It is also a learning project: event-driven programming in C with GTK4, GLib/GObject, D-Bus and libpulse, all new to me.

> 🚧 **Work in progress.** Milestone 0 (research) and milestone 1 (the first window) done on 8 Nov 2026, milestone 2 (live stats) on 10 Nov: the window shows **RAM**, **CPU %**, **CPU temperature and fan**, **GPU frequency** and **battery / charger**, updated every second, at no measurable fps cost. Now: milestone 3, the timer and healthy-gaming features (steps 1–8 done: settings file and session plan 11 Nov; countdown, Pause, End session, break screen and eye cue 17 Nov; notifications and the bell 18 Nov; plan-page choices 19 Nov).

**Milestone 2 (10 Nov):** the first working prototype, every stat live and refreshed once a second.

<img src="assets/historic/first_working_prototype.png" alt="Gaming Hub window showing RAM, CPU, temperature and fan, GPU frequency and battery" width="300">

#### Brief overview:

The window is snapped to one half of the laptop screen (Super+Left/Right), next to the YouTube Music app on the other half.

| Panel | Shows / does | Data source |
| --- | --- | --- |
| Stats | CPU % and frequency, GPU frequency and busy %, temperatures, RAM, battery and charger | `/proc`, `/sys/class/drm`, `/sys/class/hwmon`, `/sys/class/power_supply` |
| Headset | Volume, mute, mic volume and mute, battery (if Linux can read it) | PipeWire through libpulse; battery TBD in milestone 0 |
| Timer | Session plan, breaks, wind-down warning, 20-20-20 eye cue, pause button | GLib timers, GtkMediaFile, GNotification |
| Power | Balanced ↔ Performance switch, charger warning | power-profiles D-Bus API (tuned-ppd), UPower |
| Session log | Game (detected, then confirmed by me at the end), start/end, play time, average fps, max temperature → CSV | Own code, `~/.local/share/gaming_hub/` |
| Game notes | Per-game card: settings that worked, "where I was / what's next" | Text files per game |
| Library | All my games (Steam, GOG/Epic through Heroic, emulators) in one list, started from the hub | Steam `appmanifest_*.acf`, Heroic's library JSON, ROM folders |
| Temp graph | Temperature over the whole session | GtkDrawingArea + Cairo |
| FPS (stretch) | Frame rate and frame time | MangoHud's log, if it can be read live |

**Starts and stops by itself:** every game launches through `gamemoderun`, and GameMode's `[custom] start=` / `end=` hooks run the hub. It is single-instance (GtkApplication), so the end hook just tells the running window to close the session. Emulators get `gamemoderun` added once to their app shortcut, so they open the hub too.

**Rule: the hub must not cost fps.** Iris Xe shares one power budget between CPU and GPU, so the hub refreshes once a second, redraws only what changed and has no animations. Checked at milestone 2 and at the end.

`Files:`

* `sources/main.c`: creates the application (`ch.bitminded.GamingHub`) and the hub's state struct, runs GTK's main loop, builds the window when the app is activated, starts the once-a-second timer (`hub_tick`), frees the struct's strings when the app ends
* `sources/inits.c`: `hub_struct_init` sets every field to a known value: `hub_computer_init` (stats), `hub_timer_init` (timer widgets to `NULL`, state) and `hub_setting_init` (the settings to their defaults)
* `sources/main_build.c`: the window's layout and shared layout helpers: `hub_build_box` (a stack of pages above the stats), `hub_image_with_label` (a theme icon at 48 px with an optional wrapping label under it), `hub_labelled` (a left-aligned label in front of a widget)
* `sources/utils.c`: `hub_setting_hub` (loads the timer settings, then labels + finding the sensor folders, then the timer widgets, once), `hub_free_hub` (frees what the struct owns)
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
  * `timer_build.c`: the timer's pages: `hub_build_plan` (three columns: checkboxes, `hub_plan_box_middle` with the three fields, alarm + Start), `hub_build_session`, `hub_build_break`
* `includes/gaming_hub.h`: the header every `.c` file includes; pulls in the others and holds the prototypes of `main.c`, `utils.c`, `inits.c` and `main_build.c`
* `includes/structs.h`: the structs: `t_hub` holds a `t_computer` (stat labels, sensor folders, CPU samples), a `t_settings` (timer values) and a `t_timer` (timer widgets and state), all by value, plus the window's `stack` and the application (`app`, borrowed: `main` owns it)
* `includes/defaults.h`: the values the hub uses when the settings file doesn't set them (`HUB_DEF_…`)
* `includes/computer.h`, `includes/timer.h`: prototypes of `sources/computer/` and `sources/timer/`
* `Makefile`: builds the hub and libft, with GTK4's flags from `pkg-config`; also writes `compile_flags.txt` for the editor
* `assets/historic/`: screenshots of the hub at each stage (`first_working_prototype.png`, milestone 2)
* `docs/research.md`: what I tested by hand before coding (headset battery over D-Bus, MangoHud logging) and the rules the code must follow

`Additionally contains:`

* `libft/`: my 42 libft (with `ft_printf` and `get_next_line`)

## Instructions

Linux with GTK4's development package (and, from milestone 5, the PulseAudio client library; PipeWire speaks its API):

```bash
# Fedora
sudo dnf install gtk4-devel pulseaudio-libs-devel
# Debian / Ubuntu
sudo apt install libgtk-4-dev libpulse-dev

make
./gaming_hub
```

The Makefile gets GTK's compiler and linker flags from `pkg-config`, so it builds on any distribution without editing paths.

**Editor support:** `make` also writes `compile_flags.txt` (one flag per line, from the same `pkg-config` output) so clangd finds GTK's headers. It is generated for each machine and not tracked in git.

**Settings (optional):** the timer works with built-in defaults. To change them, create `~/.config/gaming_hub/settings.ini`; any key you leave out keeps its default:

```ini
# Gaming Hub timer settings
[timer]
session_minutes=120
wind_down_minutes=10
break_every_minutes=50
eye_every_minutes=20
eye_seconds=20
popups=true
sound=true
```

The file is read when the window opens; no rebuild needed. It only decides what the plan page shows at first: session length, break interval, wind-down and the two checkboxes can be changed there for each session. Numbers must be whole numbers above 0; `popups` and `sound` take `true` or `false`, **lowercase** (`TRUE` is an invalid value for GLib). A missing, broken or invalid key gives one line on the terminal and its default is used.

**Notifications need a `.desktop` file** (until `make install` in milestone 4 writes it): GNOME only shows notifications from apps it can match to a `.desktop` file named after the app id. Create `~/.local/share/applications/ch.bitminded.GamingHub.desktop`:

```ini
[Desktop Entry]
Type=Application
Name=Gaming Hub
Exec=/full/path/to/gaming_hub
Icon=input-gaming
```

With Do Not Disturb on, GNOME shows no banners at all (they go to the message list); the bell still rings.

**Coming at milestone 4:** `make install` (app menu entry with an icon) and a guide to start the hub automatically with your games (Steam, Heroic, Lutris, emulators) through GameMode. Packaging (Flatpak or RPM) is a later idea.

## Milestones

Each one is usable on its own. Full plan: [`docs/game-dashboard.md`](https://github.com/TomSwb/set_up/blob/main/docs/game-dashboard.md) in my set_up repo.

- [x] 0. Research (no code): headset battery, MangoHud logging (8 Nov, see `docs/research.md`)
- [x] 1. Hello window: GtkApplication, main loop, signals, GObject basics (8 Nov)
- [x] 2. Live stats from `/proc` and `/sys` + fps-cost check (8–10 Nov)
- [ ] 3. Timer and healthy-gaming features, settings file
- [ ] 4. Auto-start with GameMode, single instance, `--end`; `make install` (app menu icon) and a guide to hook it to Steam, Heroic, Lutris and emulators
- [ ] 5. Headset volume and mic with libpulse
- [ ] 6. Power profile and charger over D-Bus
- [ ] 7. Session log, game notes, stop ritual, monthly review
- [ ] 8. Game library + launcher: Steam, GOG/Epic (Heroic) and emulator games in one list, started from the hub
- [ ] 9. Temperature graph with Cairo
- [ ] 10. FPS from MangoHud (stretch)

## Resources

#### Docs:

* [GTK4](https://docs.gtk.org/gtk4/), [GLib](https://docs.gtk.org/glib/), [GIO](https://docs.gtk.org/gio/) (GApplication, GDBus, GNotification)
* [libpulse API](https://freedesktop.org/software/pulseaudio/doxygen/)
* [power-profiles-daemon D-Bus API](https://gitlab.freedesktop.org/upower/power-profiles-daemon), [UPower](https://upower.freedesktop.org/docs/)
* [MangoHud](https://github.com/flightlessmango/MangoHud) (logging options)
* `man 5 proc`, kernel docs for `/sys/class/hwmon` and `/sys/class/power_supply`

#### Editor:

* `VSCodium`: Used to avoid Microsoft telemetry and AI-integrated temptations.

### AI:

⚠️ Disclaimer: I only ask for actual answers/solutions when I realise that I am missing theoretical knowledge.

e.g.: a new syntax or a new concept I had not explored previously.

I always write my code to the best of my abilities first.

My AI agents are set to guide me with questions as per the 42 pedagogical philosophy.

I use VSCodium on purpose to not have AI integration. This means that, except for my copy-pastes into chats for some reviews, AI does not see my code, and AI will never write my code - I do not copy-paste from AI into my code.

`Claude:`

1. Teacher: explains the new concepts of each milestone (GTK4, GLib, GObject, D-Bus, libpulse) and points me to the docs, with hints and small examples on other cases, never finished code.
2. Peer reviewer: reviews the code I paste like a 42 correction (bugs, leaks, GObject references, error handling) and asks questions that lead me to the fix.
3. Documentation: writes and keeps up to date this README and the plan in my set_up repo. The code is all mine.

## Detailed description:

### Application id

`ch.bitminded.GamingHub` (reverse of my domain `bitminded.ch`). It is only a name, nothing goes online: the system uses it to recognise the running app (single instance, milestone 4) and to name the `.desktop` file and the icon. **It never changes.** The window title, "Gaming Hub", can.

### How the program runs (milestone 1)

GTK programs are **event-driven**: `main` sets things up, then hands control to GTK's **main loop**, which sleeps until something happens and calls the functions that asked for it (callbacks).

```
main → gtk_application_new (the app object, with its id)
     → g_signal_connect(app, "activate", hub_activate)   only registers the callback
     → g_application_run → main loop starts → emits "activate" → hub_activate builds the window
                         → … waits for events …
                         → window closed → nothing holds the app → loop ends
     → g_object_unref(app) → return the exit status
```

`hub_activate` creates a `GtkApplicationWindow` (it belongs to the app and keeps it running while open), sets the title and a default size (480×720, a portrait panel; it is snapped to half the screen anyway), puts a `GtkLabel` in it and presents it. Setup first, `present` last.

### Who frees what

The docs of every function say who owns the returned object ("Return value"):

| Object | Owner | Freed by |
| --- | --- | --- |
| The app (`gtk_application_new`: "the caller takes ownership") | `main` | `g_object_unref(app)` at the end of `main` |
| The window (`gtk_application_window_new`: "owned by the called function") | GTK's list of windows | GTK, when the window is closed |
| The label (milestone 1, before the box) | the window (`gtk_window_set_child` takes its floating reference) | the window, when it is destroyed |
| The box and the stat labels (milestone 2) | the window owns the box, the box owns its labels (`gtk_box_append` takes the floating reference) | GTK, when the window is destroyed |
| The state struct `t_hub` (milestone 2) | `main` (a local variable) | nobody: it lives on `main`'s stack, and `main` is inside `g_application_run` for as long as the app runs |
| Strings from `g_file_get_contents`, `g_strdup_printf`, `g_strdup`, `g_build_filename` | the caller | `g_free`, on every path (a second pointer walks the text, only the first one is freed) |
| The sensor folder paths in `t_hub` (`temp_dir`, `fan_dir`, `gpu_dir`, `bat_dir`, `mains_dir`, milestone 2) | the struct, so `main` | `hub_free_hub` after `g_application_run` returns (`NULL` from `hub_struct_init` if never found: `g_free(NULL)` does nothing) |
| A `GDir` (`g_dir_open`) | the function that opened it | `g_dir_close` (not `g_free`, it isn't a GObject), in that same function, on every exit |
| Names from `g_dir_read_name` | the `GDir` (borrowed) | nobody: valid until the next read or `g_dir_close`, so the full path is built (an owned copy) before the folder is closed |
| Text from `hub_read_text` | the caller | `g_free`; `NULL` means it couldn't be read |
| Fixed texts (`"CPU ..."`) passed to `gtk_label_set_text` | nobody (string literals) | never freed: the label copies the text |
| `t_computer` and `t_settings` inside `t_hub` (milestone 3 restructure) | `t_hub` (they are fields, not pointers) | nobody: they are part of `t_hub`'s memory. A pointer field here would point nowhere until something allocated it |
| The config folder path from `g_get_user_config_dir()` | GLib (borrowed, `const`) | never freed or changed: GLib keeps it for the whole program |
| A `GKeyFile` (`g_key_file_new`) | `hub_load_timer_settings` | `g_key_file_free`, once, at the bottom of `hub_load_timer_settings` (the helpers only borrow it) |
| A `GError` | whoever passed `&error`, once GLib has set it | `g_error_free`, or `g_clear_error(&error)`, which also accepts a `NULL` error. Must be `NULL` before it is passed again |

Never `free()` a GObject, never unref what you don't own. Each widget is stored as the type I call most on it (`GtkWindow *` for the window, `GtkWidget *` for the label), so casts like `GTK_WINDOW()` are rare.

### Live stats (milestone 2)

**The tick.** `hub_activate` starts a timer with `g_timeout_add(1000, hub_tick, hub)` (until 17 Nov `g_timeout_add_seconds(1, …)`, see step 5): the main loop calls `hub_tick` once a second, and `hub_tick` returns `G_SOURCE_CONTINUE` to be called again (`G_SOURCE_REMOVE` would stop it). `hub_activate` also calls `hub_tick` once itself, so the labels are filled at once instead of after one second. Once a second is the fps rule: an interval of 0 would call it nonstop and eat CPU.

**One pointer for everything.** A timer callback gets one `gpointer`, so all widgets and state live in one struct, `t_hub` (since milestone 3: stats in `hub->computer`, settings in `hub->setting`), and the timer gets a pointer to it:

```
main: t_hub hub  ──&hub──▶  g_signal_connect(... "activate" ...)  ──▶  hub_activate(user_data)
                                                                          └─ g_timeout_add(1000, hub_tick, hub)     ──▶ hub_tick(user_data) every second
```

The struct lives in `main`, not in `hub_activate`: `hub_activate` returns right after building the window, so a local there would be gone (dangling pointer) when the first tick comes. `main` stays inside `g_application_run` until the app quits, so its locals live exactly as long as the app. A plain struct comes back from `gpointer` by assignment (`t_hub *hub = user_data;`); a GObject uses its check macro (`GTK_LABEL(...)`), which checks the type at run time.

**Layout.** A window has one child, so that child is a vertical `GtkBox`; each stat is a `GtkLabel` appended to it. `gtk_label_set_text` copies the text, and does nothing (no redraw) when the text hasn't changed: that is the "redraw only what changed" rule for free.

**Reading kernel files.** `/proc` and `/sys` files are text the kernel writes when you read them. `g_file_get_contents` reads one whole file into a new string (`contents`, `length` and the error are out-parameters; `NULL` for the error, because a failure every second must not spam: the label shows `...` instead). Values are found with pointers into that text, no copies: `ft_strnstr` for the key (with its colon, so `Active:` doesn't match `Active(anon):`), jump past the key, `g_ascii_strtoull` for the number. Its `endptr` says where it stopped: if it didn't move, there was no number; it is also where the next number starts.

**RAM** (`ram.c`): used = `MemTotal − MemAvailable`. `MemFree` is wrong for this: Linux fills spare RAM with file cache and drops it when a program needs memory; `MemAvailable` is the kernel's estimate of what a new program (the game) can get without swapping. Values are in KiB, shown in GiB (divide by `1024.0 * 1024.0`, the `.0` makes it a `double` division).

**CPU %** (`cpu.c`): the first line of `/proc/stat` holds counters, in 1/100 s, of time spent by all 8 threads since boot: user, nice, system, idle, iowait, irq, softirq, steal (guest columns skipped: already inside user/nice). A percentage needs two readings: CPU % = 100 × (Δtotal − Δidle) / Δtotal, with total = columns 1–8 and idle = idle + iowait. The struct keeps the previous reading (`prev_total`, `prev_idle`). First tick: nothing to compare with yet, so it stores the reading and shows `CPU -`. If Δtotal is 0 (two calls within 10 ms), it keeps the old text instead of dividing by zero. Check: one `yes > /dev/null` adds about 12.5 % (one thread of 8).

**Unknown values.** Every label has a failure text with `...` in place of the value (`RAM ... GiB`, `CPU ...`, `TEMP ... °C`, `GPU ... MHz`, `BAT ... %`), so the window never shows a stale or invented number. `CPU ...` also covers the first tick. A real `0` is different and is shown: the fan stopped at idle, the GPU asleep.

**Finding devices by name, not number.** `/sys/class/hwmon/hwmon8` today can be `hwmon7` after a reboot, and `card1` can be `card0`. So each device folder is found **once**, when the window is built, and kept in `t_hub` (state, like `prev_total`); the tick only reads one small file per value. `hub_find_dir(class_dir, file, value)` opens the class folder with `GDir`, builds `class_dir/entry/file` for each entry with `g_build_filename`, reads it, cuts the trailing `\n` (`g_strchomp`, the kernel ends these files with a newline) and compares it with `g_strcmp0` (0 = equal). It returns the folder path, or `NULL`. Used four times:

| Device | Class folder | File = value | Found here |
| --- | --- | --- | --- |
| CPU temperature | `/sys/class/hwmon` | `name` = `coretemp` | `hwmon8` |
| Fan | `/sys/class/hwmon` | `name` = `thinkpad` | `hwmon6` |
| Battery | `/sys/class/power_supply` | `type` = `Battery` | `BAT0` |
| Charger | `/sys/class/power_supply` | `type` = `Mains` | `AC` |

The GPU has no `name` file, and its connectors (`card1-eDP-1`, `card1-HDMI-A-1`, …) start with the same name, so `hub_find_gpu` takes the DRM entry that **contains** `gt_act_freq_mhz` (`g_file_test`).

**Temperature and fan** (`temp_fan.c`): `temp1_input` in the `coretemp` folder is "Package id 0", the whole chip, in millidegrees (`31000` = 31 °C, steps of 1 °C on this CPU), shown with `/ 1000.0` and `%.0f`. The Iris Xe GPU is on the same chip, so this covers it too. `fan1_input` in the `thinkpad` folder is in RPM. Each value is read once per tick and has its own `...`. The fan sits at 0 until the firmware wants it (one busy thread reached 65 °C with the fan still off); under `stress-ng --cpu 0`: short peak at the turbo start, then 60–70 °C, fan ~3800 RPM (baseline 6 Nov: 59–63 °C, ~3890 RPM).

**GPU** (`gpu.c`): `gt_act_freq_mhz` is what the GPU **actually** runs at; `gt_cur_freq_mhz` is only the driver's last request (350 while the GPU sleeps). Between frames the GPU switches off (RC6), so at the desktop or with a video (decoded by the separate media engine) it reads 0 with short peaks; in a game it stays high. The maximum comes from `gt_RP0_freq_mhz` (the hardware's highest, 1300 MHz here), read once, so the line is `GPU 1300 / 1300 MHz` at the limit and works on other GPUs.

**Battery** (`battery.c`): `capacity` (%) and `status` (`Charging`, `Discharging`, `Not charging`, `Full`) from the battery folder; `hub_read_text` returns the word. If `status` can't be read, the charger's `online` (1 = plugged in) gives `plugged in` / `on battery` instead. `Not charging` at 99 % is the ThinkPad protecting a full battery. No battery at all: `BAT ... %`.

**Formats.** `guint64` is printed with `"%" G_GUINT64_FORMAT` (a macro string glued to `"%"`), not `%lu`, which only happens to match on 64-bit Linux. A fixed text goes straight to `gtk_label_set_text` (it copies); `g_strdup_printf` only when values are formatted in.

**fps-cost check (10 Nov).** `glmark2` (fixed 3D scenes, one score), charger in, Balanced profile, screen lock held off with `gnome-session-inhibit --inhibit idle`: **3581 without the hub, 3546 with the hub open beside it (−1.0 %)**, inside normal run-to-run noise. A run where the screen locked scored 3943 (frames not displayed), so it didn't count. A real game test comes again at the end of the project.

### Settings file (milestone 3, step 1)

**Where.** `g_get_user_config_dir()` gives the user's XDG config folder (`~/.config`), so the file is `~/.config/gaming_hub/settings.ini`, built with `g_build_filename`. Each user has their own; nothing is installed or shared, and the file is optional.

**Format.** A GLib key file (INI style): `[timer]` is the group, `key=value` lines, `#` comments. `GKeyFile` reads it: `g_key_file_new` → `g_key_file_load_from_file` (flags `G_KEY_FILE_NONE`: read only, nothing kept for writing back) → `g_key_file_get_integer` per key → `g_key_file_free`.

**Defaults first.** `hub_timer_init` sets every value from `defaults.h` (`HUB_DEF_SESSION` 120, `HUB_DEF_WIND_DOWN` 10, `HUB_DEF_BREAK_MIN` 50, `HUB_DEF_EYE_CUE_MIN` 20, `HUB_DEF_EYE_CUE_LENGTH` 20). The file can only replace a value, one key at a time; a read that fails leaves the default untouched. Test values on 11 Nov: the file's `break_every_minutes=45` replaced the default 50.

**`GError`.** The last argument `&error` (a `GError *` set to `NULL` first) is how GLib says *why* something failed: a domain (which library), a code (which error) and a `message`. `g_error_matches(error, domain, code)` checks both at once and returns `FALSE` for a `NULL` error. Once set, the error is the caller's: `g_error_free`, or `g_clear_error(&error)`, which also handles `NULL`. `g_key_file_get_integer` returns 0 on failure, and 0 could be a real value, so the error is checked, not the number.

| What happens | Error | The hub |
| --- | --- | --- |
| No settings file | `G_FILE_ERROR` / `G_FILE_ERROR_NOENT` | silent, all defaults (a normal case: never configured) |
| File exists but can't be read or parsed | another file or key-file error | `Can't read file: <path>, <reason> - using default` |
| `[timer]` group or a key missing | `G_KEY_FILE_ERROR` / `GROUP_NOT_FOUND`, `KEY_NOT_FOUND` | `Not found: <key> - using default` (catches typos in key names) |
| Not a number (`=abc`) | `G_KEY_FILE_ERROR_INVALID_VALUE` | `Can't read file: <key> <reason> - using default` |
| A number ≤ 0 | none (GLib accepts it) | `Value must be higher than 0: <key> - using default` (a break every 0 minutes would fire every second) |

**One exit, frees at the bottom.** `hub_load_timer_settings` never returns early: build the path, create the key file, read the keys only if loading worked, then free the key file and the path. Every path through the function reaches the same two frees. The helpers borrow the key file and never free it; each one frees only its own `GError`.

### Session plan (milestone 3, step 2)

**The field.** A `GtkSpinButton` (a number with − / +, typing allowed) made with `gtk_spin_button_new_with_range(5, 360, 5)`: the limits are `HUB_SESSION_LOWEST`, `HUB_SESSION_HIGHEST` and `HUB_SESSION_STEP` in `defaults.h`, fixed limits rather than settings (no `DEF`). A `GtkAdjustment` behind it holds the value and limits. It is pre-filled with `gtk_spin_button_set_value` from `hub->setting.session`, which is why the settings load first in `hub_setting_hub`. `set_value` clamps to the range, so a file value of 500 shows 360. The step is only how far one click moves; breaks don't depend on it.

**The button.** `gtk_button_new_with_label("Start")`; pressing it emits `clicked`. `g_signal_connect(button, "clicked", G_CALLBACK(hub_start_clicked), hub)` connects the callback, whose signature comes from the signal's doc page: `void (GtkButton *self, gpointer user_data)`. `user_data` is the `hub` passed to `g_signal_connect`, so the callback reads the field with `gtk_spin_button_get_value_as_int` into `hub->timer.planned`. The countdown itself is step 3.

### Countdown and state machine (milestone 3, step 3)

**States.** `t_timer_state` (an `enum` in `structs.h`) names the timer's states: `TIMER_PLANNING`, `TIMER_PLAYING`, `TIMER_PAUSED`, `TIMER_BREAK`, `TIMER_ENDED`. The hub is always in exactly one, kept in `hub->timer.state`; events (a click, a tick) look at it and may move to another. The `TIMER_` prefix matters because enum names are global like `# define`s: a bare `BREAK` could clash with a library. So far: planning at start, Start → playing, time up → ended. Paused and break come in steps 4 and 5.

**The clock.** The countdown does not count ticks: `g_timeout_add_seconds` can fire late or group timers, so ticks drift. Start stores `g_get_monotonic_time()` in `hub->timer.started` (a `gint64` in microseconds from a fixed point; only differences mean anything). It is *monotonic*: it never jumps when the wall clock changes (time sync, summer/winter time). Each tick, `hub_update_timer` works out `elapsed = (now - started) / G_USEC_PER_SEC` and `left = planned * 60 - (gint)elapsed`: the cast is the one visible `gint64` → `gint` conversion, so `%d` gets ints. At `left <= 0` it shows `0:00:00` and sets `TIMER_ENDED`; from then on its first line (`if not playing, return`) stops all work.

**Showing it.** `hub_show_time(label, seconds)` splits the seconds with `/` and `%` (3600, 60), formats `"%d:%02d:%02d"` (`%02d` pads with a zero: `0:05:09`), sets the label (which copies the text) and frees the string. It is its own function because the norm allows 5 variables per function, and the countdown needs 7.


### Pause and End session (milestone 3, step 4)

**Every button checks the state.** A click only does something in the states where it makes sense; the others are ignored:

| Button | Acts when | Does |
| --- | --- | --- |
| Start | planning or ended | reads the minutes, stores the start time, → playing, shows the time at once |
| Pause | playing | stores `paused_at`, → paused, label "Resume" |
| Resume (same button) | paused | `started += now - paused_at`, → playing, label "Pause" |
| End session | playing or paused | → ended, shows `0:00:00`, label back to "Pause" |

**Leaving paused time out.** `hub_update_timer` stays `elapsed = now - started`. On Resume, `started` moves forward by the length of the pause, so the formula sees a session that started later and counts only played time. While paused, the countdown freezes by itself because `hub_update_timer` returns for every state except playing.

**Ended stays a state.** Start works again from ended, but the hub doesn't jump back to planning: the end alert (step 7) and the stop ritual (milestone 7) happen in the ended state; going back to planning will be the ritual's last step.

**Greying out (replaced in step 5).** First `hub_refresh_buttons` greyed out what couldn't be used, with `gtk_widget_set_sensitive` (a comparison is already a `gboolean`, so it goes straight in). Step 5 replaced that with pages: a button that can't be used isn't on screen.

**Decided 17 Nov:** End session stays (mainly until milestone 7 notices the game closing). No "close the game" button (killing a game loses everything since the last save; quitting is the game's job) and no extending the planned time.

### Pages and the break screen (milestone 3, step 5)

**One window, several pages.** A `GtkStack` holds several children but shows one; each has a name, and `gtk_stack_set_visible_child_name` switches instantly (no animation, fps rule). The layout:

```
window
└── main box (vertical)
    ├── stack            the part that changes
    │   ├── "plan"       alarm icon, minutes field + Start
    │   ├── "session"    countdown, controller icon, Pause/Resume + End session
    │   └── "break"      "Break", three icons (stand, roll neck & shoulders, drink water), Done
    └── stats box        always visible, under every page
```

The stats are outside the stack because a widget has only one parent, so they can't be in two pages; this way they stay visible on every page (and later under the library). Later pages: "ritual" (milestone 7) and "library" (milestone 8). For the styling step: the stats should be visible but not dominant, which one CSS class on their box can do.

**State → page.** `hub_show_timer_page` picks the page from the state (planning or ended → plan, playing or paused → session, break → break) and is called after every state change, including once at the end of `hub_build_box`, the first moment the stack exists (calling it earlier, from `hub_timer_manager`, would pass a `NULL` stack).

**When a break comes.** `hub->timer.next_break` is the elapsed second of the next break: `break_min * 60` at Start. Each tick while playing, `hub_update_timer` checks `elapsed >= next_break` and switches to break. Breaks **count** toward the session (decided 17 Nov): the countdown keeps running in the break state, and the session can end during one. Done sets `next_break = elapsed + break_min * 60`, so every break is followed by a full `break_min` of play. To test without waiting: `break_every_minutes=1` in the settings file.

**Icons.** `gtk_image_new_from_icon_name` takes a name from the icon theme (Adwaita: `alarm-symbolic`, `input-gaming-symbolic`, `preferences-desktop-accessibility-symbolic`, `object-rotate-right-symbolic`, `weather-showers-symbolic`); `gtk_image_new_from_file` takes a file path, relative to the folder the hub was started from. The Adwaita icons are placeholders until custom ones arrive, which will come as a GResource (compiled into the program) or as installed named icons, not as relative paths.

**C or CSS.** Where things go is C (which boxes and pages, orientation, `gtk_widget_set_halign`, `gtk_box_set_homogeneous`, label wrapping); what they look like is CSS (colours, fonts, margins, borders). GTK CSS can't position widgets.

**The tick.** Since step 5 the tick is `g_timeout_add(1000, …)` instead of `g_timeout_add_seconds(1, …)`: the seconds version lines its ticks up with other programs' timers to save power and can fire almost a second late, so the countdown sometimes stayed on a number and then skipped one. Still once a second.

### Eye cue (milestone 3, step 6)

**What it does.** Every `eye_min` minutes of play (20), a line under the countdown, "Look at something 6m away for 20 seconds", shows for `eye_sec` seconds (20), then hides. No page switch, nothing to click: the 20-20-20 rule, quieter than the break.

**Showing and hiding.** `gtk_widget_set_visible(widget, FALSE)` hides a widget: it stays in its box but takes no space and isn't drawn. The label is created hidden in `hub_timer_manager`, and Start hides it again (a cue could still be showing from the last session).

**Two schedules, one idea.** Like `next_break`, `next_eye` is the elapsed second of the next cue (`eye_min * 60` at Start), and `eye_until` the second it hides. Each tick while playing, `hub_update_eye`: if `elapsed >= next_eye`, show it, `eye_until = elapsed + eye_sec`, and `next_eye += eye_min * 60` (*moved on by*, not *set to*: set to, it would fire on every tick after the first cue). If `elapsed >= eye_until`, hide it (hiding a hidden widget changes nothing, so this is harmless before the first cue). The eye check runs **after** the break check, so when both are due on the same second (minute 60 with 20/60) the break wins and no cue shows during it. To test: `eye_every_minutes=1`, `eye_seconds=5`.

**Text from the setting.** `hub_show_eye` builds the text with `g_strdup_printf` from `eye_sec`, so changing the setting changes the text too.

**Order in Start.** `hub_update_timer` is the last line of `hub_start_clicked`: it reads `next_eye`, so everything it uses has to be set first.

### Notifications and the bell (milestone 3, step 7)

**What reaches you.** The hub sits on the laptop screen while the game is on another, so a change in the window is easy to miss. So:

| Moment | Notification (id) | Bell |
| --- | --- | --- |
| Eye cue | "Eye rest" (`eye`) | no (it's meant to be quiet) |
| Break | "Break time" (`break`) | yes |
| Wind-down (`wind_down` minutes before the end, once) | "Wind down: N minutes left - head for a save point" (`wind-down`) | yes |
| Session over (time up; not End session, which is my own choice) | "Session over" (`end`) | yes |

**GNotification.** `g_notification_new(title)` + `g_notification_set_body`, sent with `g_application_send_notification(app, id, n)`, which takes its own reference, so the hub unrefs its one at once. The same id replaces the previous notification instead of stacking (so the eye cues don't pile up). The `GApplication` is kept in `hub->app`, a borrowed pointer: `main` owns it.

**Why nothing showed at first (18 Nov).** Two causes, found one at a time: Do Not Disturb was on (`gsettings get org.gnome.desktop.notifications show-banners` → `false`; `notify-send` showed nothing either), and GNOME drops notifications from an app it can't match to a `.desktop` file named after the app id (tested: removing the file stopped them again). Fullscreen games: GNOME doesn't show banners over a fullscreen window on the same screen either, so the bell is the part that reliably reaches you.

**The bell.** `gtk_media_file_new_for_filename(HUB_SOUND)` (the freedesktop theme's `bell.oga`, chosen by ear with `pw-play`) is created once in `hub_timer_manager`; `hub_alert` rewinds it (`gtk_media_stream_seek(…, 0)`) and plays it; playback runs in the background. The stream is owned by the hub: `hub_free_hub` unrefs it if it was created. If the file is missing, the stream holds an error and plays nothing. Later: the hub's own sound as a GResource.

**Wind-down once.** `hub->timer.wind_warned` is reset at Start and set when the warning fires, so the last `wind_down` minutes don't warn every second. Every text built with `g_strdup_printf` is built and freed inside the `if` where it's used, not on every tick.

### Plan-page choices (milestone 3, step 8)

**The plan page** has three columns, split evenly with `gtk_box_set_homogeneous`:

```
[ ] Pop-ups     Session length: [120]     [alarm]
[ ] Sound       Break every:    [ 50]     [Start]
                Wind down at:   [ 10]
```

Everything is pre-filled from the settings and read at Start by `hub_read_plan` into the values the session uses (`hub->timer.planned`, `hub->setting.break_min`, `hub->setting.wind_down`, `hub->timer.popups`, `hub->timer.sound_on`). The settings file is only read when the hub opens; after that the fields keep their last values, so the next session starts with what was chosen last. Decided 17 Nov (instead of a Settings page): per-session choices, nothing written back to the file.

**Limits and steps** (`defaults.h`): session 5–360 by 5, break every 5–90 by 5, wind-down 1–30 by 1. `set_value` clamps a file value outside the range (a test file's `break_every_minutes=2` showed 5).

**Checkboxes.** `GtkCheckButton` with `gtk_check_button_set_active` / `get_active`. `hub_alert` notifies only if `popups` and rings only if `sound_on`; the eye cue goes through `hub_notify`, so unticking Pop-ups silences it too. Their defaults (`HUB_DEF_POPUPS`, `HUB_DEF_SOUND`, both `TRUE`) can be set in the file as `popups=` / `sound=`, read with `g_key_file_get_boolean` (no range check: `FALSE` is a valid 0).

**Layout lessons.** Columns lined up with `gtk_widget_set_hexpand(label, TRUE)` + `gtk_label_set_xalign(label, 0)` in `hub_labelled`: every row in a vertical box gets the widest row's width, so the stretching label pushes each field to the same right edge. `valign` only matters when a widget gets more height than it needs (a homogeneous box, or a taller parent); in a normal vertical box each child gets exactly its natural height, so centring does nothing there. A GtkSpinButton's − / + can't be removed; CSS can restyle them at the styling step (`spinbutton > button`), or `GtkDropDown` / `GtkScale` could replace them.

**Breaks vs wind-down, on screen.** The countdown shows time *left*; breaks count time *played*, wind-down counts time *left*. A 6-minute session with "Break every 5" gets its break when the countdown reads 1:00; "Wind down at 5" fires at 5:00 left.
