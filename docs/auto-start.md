# Auto-start and install (milestone 4)

Single instance, the `--end` command line, starting with GameMode, `make install`, step by step. Back to the [README](../README.md).

## Single instance (milestone 4, step 1, 20 Nov)

**Primary and remote.** The app id makes the hub single-instance by itself: the first `gaming_hub` registers `ch.bitminded.GamingHub` on the session's D-Bus and becomes the **primary** instance. A second `gaming_hub` finds the name taken and becomes a **remote** instance: it builds nothing, sends one message to the primary ("activate") and exits at once.

**`activate` can come more than once.** The primary emits `activate` for every launch, not only the first one. Until 20 Nov, `hub_activate` built everything again on the second launch: a second window, `hub_setting_hub` a second time (new labels and timer widgets replaced the pointers in `t_hub`, so the first window froze; the sensor folder paths were found again and the old strings leaked) and a second `g_timeout_add` (the tick twice a second, against the fps rule). Tested with two terminals: a second, identical window.

**The fix.** `hub_activate` first asks GTK whether the app already has a window, `gtk_application_get_active_window(self)` (borrowed: "owned by the instance", never unref'd). If there is one, it is brought to the front with `gtk_window_present` and the function returns; the early return is the very first thing, so it skips nothing that was set up. Otherwise the window is built as before. Asking GTK rather than keeping a `window` field in `t_hub`: GTK's list of windows is always right, a field of my own would still point at a closed window. Checked: a second launch brings the window forward, no new window.
