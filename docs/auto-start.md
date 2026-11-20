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
