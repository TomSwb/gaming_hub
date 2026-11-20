# Auto-start and install (milestone 4)

Single instance, the `--end` command line, starting with GameMode, `make install`, step by step. Back to the [README](../README.md).

## Single instance (milestone 4, step 1, 20 Nov)

**Primary and remote.** The app id makes the hub single-instance by itself: the first `gaming_hub` registers `ch.bitminded.GamingHub` on the session's D-Bus and becomes the **primary** instance. A second `gaming_hub` finds the name taken and becomes a **remote** instance: it builds nothing, sends one message to the primary ("activate") and exits at once.

**`activate` can come more than once.** The primary emits `activate` for every launch, not only the first one. Until 20 Nov, `hub_activate` built everything again on the second launch: a second window, `hub_setting_hub` a second time (new labels and timer widgets replaced the pointers in `t_hub`, so the first window froze; the sensor folder paths were found again and the old strings leaked) and a second `g_timeout_add` (the tick twice a second, against the fps rule). Tested with two terminals: a second, identical window.

**The fix.** `hub_activate` first asks GTK whether the app already has a window, `gtk_application_get_active_window(self)` (borrowed: "owned by the instance", never unref'd). If there is one, it is brought to the front with `gtk_window_present` and the function returns; the early return is the very first thing, so it skips nothing that was set up. Otherwise the window is built as before. Asking GTK rather than keeping a `window` field in `t_hub`: GTK's list of windows is always right, a field of my own would still point at a closed window. Checked: a second launch brings the window forward, no new window.

## `--end` on the command line (milestone 4, step 2, 20 Nov)

**The primary gets every command line.** With `G_APPLICATION_DEFAULT_FLAGS` a remote only sends "activate" and its arguments are lost. With `G_APPLICATION_HANDLES_COMMAND_LINE` the remote forwards its whole command line, and the primary emits **`command-line`** instead of `activate`, for the first launch too. `activate` is no longer emitted by itself: `hub_command_line` calls `g_application_activate(self)` when it wants the window (and `hub_activate` still builds it, once). Its return value is the exit status of the process that was launched (the remote, or the first launch). Tested with two terminals: output printed by the handler appears in the hub's terminal, never in the one where `--end` was typed: the work happens in the primary, the remote returns at once.

**Declaring the option.** `g_application_add_main_option(app, "end", 0, G_OPTION_FLAG_NONE, G_OPTION_ARG_NONE, "Ends the session", NULL)` before `g_application_run`: a switch (`ARG_NONE`, no value), no short form (`short_name` is a `char`, so `0`, not `NULL`), listed by `gaming_hub --help`. GLib parses it into a `GVariantDict` on the command line: `g_application_command_line_get_options_dict` (borrowed, owned by the command line) and `g_variant_dict_contains(options, "end")`.

**What `--end` does.** It calls `hub_stop_clicked(NULL, hub)`, the End session button's callback (it never uses its button, so `NULL` is safe as long as that stays true). Since 20 Nov that ends a session that is playing, paused **or in a break** (closing the game during a break is natural; before, the countdown would have run on and rung "Session over" for a closed game). The window stays open on the plan page.

| Launch | Hub running? | Result |
| --- | --- | --- |
| `gaming_hub` | no | opens the window |
| `gaming_hub` | yes | brings the window to the front (step 1) |
| `gaming_hub --end` | yes | the running session ends; the command returns at once |
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
