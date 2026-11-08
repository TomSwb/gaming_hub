# Gaming_Hub - a program by TomSwb

## Description

#### Goal:

A small GTK4 window for my laptop screen while I game on the TV or projector. It opens when a game starts, shows what I care about during a session (system stats, headset, break timer, power), keeps a log of my sessions, and acts as the break reminder of my Healthy Gaming Charter.

It is also a learning project: event-driven programming in C with GTK4, GLib/GObject, D-Bus and libpulse, all new to me.

> 🚧 **Work in progress.** Milestone 0 (research) and milestone 1 (the first window) done on 8 Nov 2026: the hub opens a "Gaming Hub" window with a label. Next: live stats (milestone 2).

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
| Temp graph | Temperature over the whole session | GtkDrawingArea + Cairo |
| FPS (stretch) | Frame rate and frame time | MangoHud's log, if it can be read live |

**Starts and stops by itself:** every game launches through `gamemoderun`, and GameMode's `[custom] start=` / `end=` hooks run the hub. It is single-instance (GtkApplication), so the end hook just tells the running window to close the session. Emulators get `gamemoderun` added once to their app shortcut, so they open the hub too.

**Rule: the hub must not cost fps.** Iris Xe shares one power budget between CPU and GPU, so the hub refreshes once a second, redraws only what changed and has no animations. Checked at milestone 2 and at the end.

`Files:`

* `main.c`: creates the application (`ch.bitminded.GamingHub`), runs GTK's main loop, builds the window when the app is activated
* `includes/gaming_hub.h`: shared header
* `Makefile`: builds the hub and libft, with GTK4's flags from `pkg-config`; also writes `compile_flags.txt` for the editor
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

**Coming at milestone 4:** `make install` (app menu entry with an icon) and a guide to start the hub automatically with your games (Steam, Heroic, Lutris, emulators) through GameMode. Packaging (Flatpak or RPM) is a later idea.

## Milestones

Each one is usable on its own. Full plan: [`docs/game-dashboard.md`](https://github.com/TomSwb/set_up/blob/main/docs/game-dashboard.md) in my set_up repo.

- [x] 0. Research (no code): headset battery, MangoHud logging (8 Nov, see `docs/research.md`)
- [x] 1. Hello window: GtkApplication, main loop, signals, GObject basics (8 Nov)
- [ ] 2. Live stats from `/proc` and `/sys` + fps-cost check
- [ ] 3. Timer and healthy-gaming features, settings file
- [ ] 4. Auto-start with GameMode, single instance, `--end`; `make install` (app menu icon) and a guide to hook it to Steam, Heroic, Lutris and emulators
- [ ] 5. Headset volume and mic with libpulse
- [ ] 6. Power profile and charger over D-Bus
- [ ] 7. Session log, game notes, stop ritual, monthly review
- [ ] 8. Temperature graph with Cairo
- [ ] 9. FPS from MangoHud (stretch)

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
| The label | the window (`gtk_window_set_child` takes its floating reference) | the window, when it is destroyed |

Never `free()` a GObject, never unref what you don't own. Each widget is stored as the type I call most on it (`GtkWindow *` for the window, `GtkWidget *` for the label), so casts like `GTK_WINDOW()` are rare.
