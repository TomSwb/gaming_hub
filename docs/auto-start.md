# Auto-start and install (milestone 4)

Single instance, the `--end` command line, starting with GameMode, `make install`, step by step. Back to the [README](../README.md).

## Single instance (milestone 4, step 1, 20 Nov)

**Primary and remote.** The app id makes the hub single-instance by itself: the first `gaming_hub` registers `ch.bitminded.GamingHub` on the session's D-Bus and becomes the **primary** instance. A second `gaming_hub` finds the name taken and becomes a **remote** instance: it builds nothing, sends one message to the primary ("activate") and exits at once.

**`activate` can come more than once.** The primary emits `activate` for every launch, not only the first one. Until 20 Nov, `hub_activate` built everything again on the second launch: a second window, `hub_setting_hub` a second time (new labels and timer widgets replaced the pointers in `t_hub`, so the first window froze; the sensor folder paths were found again and the old strings leaked) and a second `g_timeout_add` (the tick twice a second, against the fps rule). Tested with two terminals: a second, identical window.

**The fix.** `hub_activate` first asks GTK whether the app already has a window, `gtk_application_get_active_window(self)` (borrowed: "owned by the instance", never unref'd). If there is one, it is brought to the front with `gtk_window_present` and the function returns; the early return is the very first thing, so it skips nothing that was set up. Otherwise the window is built as before. Asking GTK rather than keeping a `window` field in `t_hub`: GTK's list of windows is always right, a field of my own would still point at a closed window. Checked: a second launch brings the window forward, no new window.

## `--end` on the command line (milestone 4, step 2, 20 Nov)

**The primary gets every command line.** With `G_APPLICATION_DEFAULT_FLAGS` a remote only sends "activate" and its arguments are lost. With `G_APPLICATION_HANDLES_COMMAND_LINE` the remote forwards its whole command line, and the primary emits **`command-line`** instead of `activate`, for the first launch too. `activate` is no longer emitted by itself: `hub_command_line` calls `g_application_activate(self)` when it wants the window (and `hub_activate` still builds it, once). Its return value is the exit status of the process that was launched (the remote, or the first launch). Tested with two terminals: output printed by the handler appears in the hub's terminal, never in the one where `--end` was typed: the work happens in the primary, the remote returns at once.

**Declaring the option.** `g_application_add_main_option(app, "end", 0, G_OPTION_FLAG_NONE, G_OPTION_ARG_NONE, "Ends the session", NULL)` before `g_application_run`: a switch (`ARG_NONE`, no value), no short form (`short_name` is a `char`, so `0`, not `NULL`), listed by `gaming_hub --help`. GLib parses it into a `GVariantDict` on the command line: `g_application_command_line_get_options_dict` (borrowed, owned by the command line) and `g_variant_dict_contains(options, "end")`.

**What `--end` did in step 2** (changed in step 5a, below). It called `hub_stop_clicked(NULL, hub)`, the End session button's callback (it never uses its button, so `NULL` is safe as long as that stays true). Since 20 Nov that ends a session that is playing, paused **or in a break** (closing the game during a break is natural; before, the countdown would have run on and rung "Session over" for a closed game). The window stays open on the plan page.

| Launch | Hub running? | Result |
| --- | --- | --- |
| `gaming_hub` | no | opens the window |
| `gaming_hub` | yes | brings the window to the front (step 1) |
| `gaming_hub --end` | yes | the running session ends; the command returns at once (until step 5a: since then it moves the session to "Between Games") |
| `gaming_hub --end` | no | nothing opens: this launch becomes the primary, the state is planning (from `hub_struct_init`), so `hub_stop_clicked` touches no widget (they were never built), and with no window the app quits at once |

## Starting with GameMode (milestone 4, step 3, 20 Nov)

**The hooks.** In `~/.config/gamemode.ini`, section `[custom]`: `start=` runs when the **first** game registers with GameMode (`gamemoderun`), `end=` when the **last** one unregisters (the game exits), `script_timeout=` is how long GameMode waits for each (10 s by default). GameMode 1.8.2 runs each line through `/bin/sh -c` (so `;`, `&`, `>>` work) and as me.

**GameMode waits for its hooks**, tested with a fake game (`time gamemoderun sleep 5`) and fake hubs (`sleep 30`, then a line written to a log file):

| `start=` | Game took | Hook | Why |
| --- | --- | --- | --- |
| `sleep 30; date …` | 15 s | killed after 10 s (`Child process timed out for /bin/sh, killing`) | GameMode waits for the script to end, up to `script_timeout`, before the game goes on |
| `(sleep 30; date …) &` | 15 s | survived (line written 30 s after launch) | the shell exits at once, but GameMode reads the script's output through a **pipe** until it closes, and the background child inherited that pipe |
| `(sleep 30; date …) > /dev/null 2>&1 &` | **5 s** | **survived** | the background part has its own output, so nothing holds GameMode's pipe: the hook ends at once |

GameMode only kills the shell at the timeout, not its background children. A first try that "returned at once" was a syntax error (a missing `)`): `Script […] failed with error -1`, exit code 2. A fast return only counts if the command ran: check GameMode's log, `journalctl --user -u gamemoded`. (Its info lines arrive in batches, so their timestamps can be late; the ERROR lines are immediate.)

**The hub's lines** (full path until `make install`, step 4):

```ini
[custom]
start=/full/path/to/gaming_hub > /dev/null 2>&1 &
end=/full/path/to/gaming_hub --end
```

`end=` needs no `&`: `--end` is a remote launch that returns at once (step 2). Tested with `time gamemoderun sleep 60`: the hub opened at once (GameMode's service can open windows: it runs in my graphical session), a session started in it ended by itself when the fake game ended, the window stayed open, and the game was not delayed.

**Seen on the way (not the hub's problem):** GameMode's own tweaks need root through polkit, and `pkexec` refuses them (`Not authorized`, exit code 127): the CPU governor (`desiredgov=powersave` in `[general]`) and `split_lock_mitigate` never applied. Noted in the set_up log to look into separately.

## `make install` and `make uninstall` (milestone 4, step 4, 20 Nov)

**Where things go** (the XDG layout, per user, no root), all under a **prefix**, `~/.local` by default:

| What | Where |
| --- | --- |
| The program | `${PREFIX}/bin/gaming_hub` (`~/.local/bin` is on the PATH on Fedora) |
| The menu entry | `${PREFIX}/share/applications/ch.bitminded.GamingHub.desktop` (named after the app id: notifications need that) |
| The icon (when the logo arrives) | `${PREFIX}/share/icons/hicolor/scalable/apps/ch.bitminded.GamingHub.svg` |

**Variables.** `PREFIX ?= ${HOME}/.local` (`?=`: only if not already set, so `make install PREFIX=…` wins; `${HOME}` comes from the environment, which make turns into variables; `~` is a shell shortcut and isn't expanded in make variables), then `BIN_DIR`, `APP_DIR` (folders, no trailing `/`) and `DESKTOP` (only the file name); a rule glues them: `${APP_DIR}/${DESKTOP}`.

**`install: all`.** First tried as `install: ${NAME}`: after `make fclean`, `make install` stopped with `No rule to make target 'libft/libft.a'`, because only the phony `libft` rule builds it and only `all` asks for that. Depending on `all` builds exactly what `make` builds. Then:

- `install -D -m 755` copies the program: `-D` creates missing parent folders, `-m 755` makes it executable (`rwxr-xr-x`).
- `mkdir -p` + `printf '%s\n' '[Desktop Entry]' 'Type=Application' 'Name=Gaming Hub' 'Comment=Gaming session companion' 'Exec=${BIN_DIR}/${NAME}' 'Icon=input-gaming' 'Categories=Game;' > …`: the `.desktop` file is written by the Makefile, not kept in git, because `Exec=` needs the installed path, known only at install time (make replaces the variables before the shell sees the single quotes). Checked with `desktop-file-validate` (silent = valid). `Categories` is plural and the list ends with `;`.
- `-update-desktop-database ${APP_DIR}`: refreshes the folder's index; the `-` lets make carry on if the tool is missing.

**`uninstall`** removes exactly the two files (`rm -f`: no error if already gone), never the shared folders, and refreshes the index.

**The menu kept the old entry.** After `make install` replaced the hand-made `.desktop` file (same name, new `Exec=`), the app menu still started the repo copy (`pgrep -a gaming_hub` shows each process's full command line). `gio launch <file.desktop>` (reads the file directly) started the installed one, so the file was right: the running GNOME Shell kept its old copy of the entry, even after `update-desktop-database`. After a restart the menu starts `~/.local/bin/gaming_hub`. Only matters when `Exec=` changes, so once. (A first `pgrep` check was misleading: the hub from the GameMode test was still open, so the menu's launch became a remote and just presented that window.)

## A real Steam game (milestone 4, step 5, 20 Nov)

The GameMode lines use the installed hub by name: `systemctl --user show-environment` shows `~/.local/bin` first in the PATH that systemd gives user services (GNOME passes the login environment on), and `readlink /proc/<pid>/exe` (the kernel's link to a process's real program) confirmed `~/.local/bin/gaming_hub`. A Steam game with the launch option `gamemoderun %command%`: the hub opened about half a second before the game's window, and the session reacted about half a second after the game's window closed.

Things learned on the way:
- **Which screen:** on Wayland an app can't choose where its window opens; GNOME usually uses the screen with the pointer or the focused window, so the hub opened on the TV. Super+Shift+←/→ moves a window to the other screen, then Super+←/→ snaps it.
- **Launch options are per game:** Steam has no global setting. `gamemoderun %command%` is added when I start playing a game; MangoHud can stay in front of it (`mangohud gamemoderun %command%`) for the fps overlay and log, which the hub doesn't replace (fps is milestone 9, and would read MangoHud's log).

## Between games: `--end` and `--start` (milestone 4, step 5a, 20 Nov)

**Why.** With `--end` ending the session, closing one game to open another started a fresh countdown on every switch: against the charter, where a session is "how long I play tonight". Decided 20 Nov:

| Moment | Command | Hub |
| --- | --- | --- |
| A game starts, hub not running | `--start` | opens on the **plan page** (also after milestone 7) |
| A game closes during a session | `--end` | **"Between Games"** page; the **countdown keeps running**, like during a break. From milestone 7: the library with the timer on top |
| The next game starts | `--start` | back to the playing page, same session and countdown |
| From the app menu | (none) | opens or comes to the front, nothing else |
| End session (any page) or time up | | the session ends |

**A new state, `TIMER_TRANSIT`** (session open, no game running, countdown runs), not a reuse of `TIMER_PAUSED`: a manual pause freezes the countdown and shows Resume, and from milestone 7 the two need different pages. Both commands leave a manual pause alone.

- **`hub_game_closed`** (`--end`, in the new `sources/game/game_events.c`): from playing, paused or break → transit. Coming from paused, the pause is first taken out of the elapsed time (`started += now - paused_at`, as Resume does): `hub_update_timer` computes `elapsed = now - started` again once the countdown runs, so without it the whole pause would suddenly count as played. Also resets the Pause button's label and hides the eye cue.
- **`hub_game_started`** (`--start`): transit → playing. If the break time passed between games, it counts as taken: `next_break = elapsed + break_min * 60`, but only `if (elapsed >= next_break)`, otherwise switching games often would push the breaks away forever.
- **`hub_command_line`**: `--end` → `hub_game_closed`, no window; otherwise activate first (so the window exists), then `--start` → `hub_game_started`. On the very first launch the state is planning, so `--start` just shows the plan page.
- **`hub_timer_running`** (`timer/timer_utils.c`): playing, break or transit; the countdown runs only then (and it kept `hub_update_timer` at 25 lines).
- **The page** (`hub_build_transit`): "Between Games", its own countdown label and its own End session button. A widget has one parent: `countdown` and `done` were already on other pages, so appending them again is refused (a `gtk_widget_get_parent (child) == NULL` CRITICAL) and the page got neither. `hub_update_timer` writes the time to both countdown labels; a label on a hidden page isn't drawn, so it costs nothing.
- **Between games:** no break and no eye cue start (both need playing); wind-down and "Session over" still fire.
- `sources/game/` is a new area: games coming and going (milestone 7's launcher goes there too).

Tested with two fake games in a row (`gamemoderun sleep 20` twice): plan page on the first launch, Between Games when it ended, the same countdown when the second started, End session from the Between Games page back to the plan page. The GameMode lines:

```ini
[custom]
start=gaming_hub --start > /dev/null 2>&1 &
end=gaming_hub --end
```

## Emulators (milestone 4, step 5, 20 Nov)

Emulators don't go through Steam, so their app-menu entries start them through GameMode. All of mine but OpenGOAL are system Flatpaks (`flatpak list --app`), whose `.desktop` files are in `/var/lib/flatpak/exports/share/applications/` with `Exec=/usr/bin/flatpak run …`.

- **Does `gamemoderun` reach a Flatpak?** It works by `LD_PRELOAD`, which may not get into the sandbox. Tested: `gamemoderun flatpak run org.DolphinEmu.dolphin-emu` opened the hub, `gamemoded -s` said `gamemode is active`, and about 10 s after Dolphin closed `inactive` (a first check right after closing still said active: GameMode notices a process that didn't unregister itself with a periodic check, every 5 s by default). So GameMode follows the emulator.
- **The menu entries:** a `.desktop` file in `~/.local/share/applications/` with the same name **overrides** the system one, which stays untouched. Checked first: none of them has `DBusActivatable=true` (GNOME would start it over D-Bus and ignore `Exec=`) or extra actions, and no file of the same name existed in my folder. Then, for each id, `sed 's|^Exec=/usr/bin/flatpak|Exec=gamemoderun /usr/bin/flatpak|' <system file> > ~/.local/share/applications/<id>.desktop`: Dolphin, PrimeHack, PCSX2, PPSSPP, RetroArch, RMG and mGBA. OpenGOAL's launcher entry was already my own file (an AppImage): dated backup, then `sed -i` in place. The launcher starts the game as a child, which inherits GameMode, so its session goes between games when both are closed.
- **Tested from the app menu:** each one opened the hub (this time GNOME Shell picked up the changed entries without a re-login).
- **Not hooked:** ES-DE (an emulator front end): not used; the hub's own library (milestone 7) stays as planned.
- fish note: a wildcard that matches nothing is an error in fish (`No matches for wildcard`), not an empty list as in bash.

## Wrap-up (20 Nov)

- `--help` texts say what the options do now ("A game closed: the session goes between games", "A game started: the session carries on"), and a norminette pass over the code (a comment after the last value of an enum confused norminette's parser into about 70 follow-up errors).
- **Time:** about 3.5 h of focused work for the whole milestone (17:23 – 21:11, with a short detour to make the Steam drive mount itself again after a replug), estimate 6–8 h. Step 1 ~0.25 h, step 2 ~0.5 h, step 3 ~0.5 h, step 4 ~0.75 h, step 5 (Steam and emulators) ~0.5 h, step 5a (between games, designed during the session) ~1 h.
- **Left for later:** the app icon is the theme's `input-gaming` until the logo arrives (then `make install` also installs `ch.bitminded.GamingHub.svg` into `${PREFIX}/share/icons/hicolor/scalable/apps/`); from milestone 7 the "Between Games" page becomes the library with the timer on top.
