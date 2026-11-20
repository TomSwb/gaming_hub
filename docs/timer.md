# Timer and healthy-gaming features (milestone 3)

Settings file, session plan, countdown, pages, eye cue, notifications and plan-page choices, step by step. Back to the [README](../README.md).

## Settings file (milestone 3, step 1)

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

## Session plan (milestone 3, step 2)

**The field.** A `GtkSpinButton` (a number with − / +, typing allowed) made with `gtk_spin_button_new_with_range(5, 360, 5)`: the limits are `HUB_SESSION_LOWEST`, `HUB_SESSION_HIGHEST` and `HUB_SESSION_STEP` in `defaults.h`, fixed limits rather than settings (no `DEF`). A `GtkAdjustment` behind it holds the value and limits. It is pre-filled with `gtk_spin_button_set_value` from `hub->setting.session`, which is why the settings load first in `hub_setting_hub`. `set_value` clamps to the range, so a file value of 500 shows 360. The step is only how far one click moves; breaks don't depend on it.

**The button.** `gtk_button_new_with_label("Start")`; pressing it emits `clicked`. `g_signal_connect(button, "clicked", G_CALLBACK(hub_start_clicked), hub)` connects the callback, whose signature comes from the signal's doc page: `void (GtkButton *self, gpointer user_data)`. `user_data` is the `hub` passed to `g_signal_connect`, so the callback reads the field with `gtk_spin_button_get_value_as_int` into `hub->timer.planned`. The countdown itself is step 3.

## Countdown and state machine (milestone 3, step 3)

**States.** `t_timer_state` (an `enum` in `structs.h`) names the timer's states: `TIMER_PLANNING`, `TIMER_PLAYING`, `TIMER_PAUSED`, `TIMER_BREAK`, `TIMER_ENDED`. The hub is always in exactly one, kept in `hub->timer.state`; events (a click, a tick) look at it and may move to another. The `TIMER_` prefix matters because enum names are global like `# define`s: a bare `BREAK` could clash with a library. So far: planning at start, Start → playing, time up → ended. Paused and break come in steps 4 and 5.

**The clock.** The countdown does not count ticks: `g_timeout_add_seconds` can fire late or group timers, so ticks drift. Start stores `g_get_monotonic_time()` in `hub->timer.started` (a `gint64` in microseconds from a fixed point; only differences mean anything). It is *monotonic*: it never jumps when the wall clock changes (time sync, summer/winter time). Each tick, `hub_update_timer` works out `elapsed = (now - started) / G_USEC_PER_SEC` and `left = planned * 60 - (gint)elapsed`: the cast is the one visible `gint64` → `gint` conversion, so `%d` gets ints. At `left <= 0` it shows `0:00:00` and sets `TIMER_ENDED`; from then on its first line (`if not playing, return`) stops all work.

**Showing it.** `hub_show_time(label, seconds)` splits the seconds with `/` and `%` (3600, 60), formats `"%d:%02d:%02d"` (`%02d` pads with a zero: `0:05:09`), sets the label (which copies the text) and frees the string. It is its own function because the norm allows 5 variables per function, and the countdown needs 7.


## Pause and End session (milestone 3, step 4)

**Every button checks the state.** A click only does something in the states where it makes sense; the others are ignored:

| Button | Acts when | Does |
| --- | --- | --- |
| Start | planning or ended | reads the minutes, stores the start time, → playing, shows the time at once |
| Pause | playing | stores `paused_at`, → paused, label "Resume" |
| Resume (same button) | paused | `started += now - paused_at`, → playing, label "Pause" |
| End session | playing or paused | → ended, shows `0:00:00`, label back to "Pause" |

**Leaving paused time out.** `hub_update_timer` stays `elapsed = now - started`. On Resume, `started` moves forward by the length of the pause, so the formula sees a session that started later and counts only played time. While paused, the countdown freezes by itself because `hub_update_timer` returns for every state except playing.

**Ended stays a state.** Start works again from ended, but the hub doesn't jump back to planning: the end alert (step 7) and the stop ritual (planned then as milestone 7, dropped 20 Nov) happen in the ended state; going back to planning will be the ritual's last step.

**Greying out (replaced in step 5).** First `hub_refresh_buttons` greyed out what couldn't be used, with `gtk_widget_set_sensitive` (a comparison is already a `gboolean`, so it goes straight in). Step 5 replaced that with pages: a button that can't be used isn't on screen.

**Decided 17 Nov:** End session stays (mainly until the hub notices the game closing: done in milestone 4, where a closing game moves the session to "Between Games"). No "close the game" button (killing a game loses everything since the last save; quitting is the game's job) and no extending the planned time.

## Pages and the break screen (milestone 3, step 5)

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

The stats are outside the stack because a widget has only one parent, so they can't be in two pages; this way they stay visible on every page (and later under the library). Later pages: "transit" (between games, added in milestone 4) and "library" (milestone 7); a "ritual" page was planned too, dropped 20 Nov. For the styling step: the stats should be visible but not dominant, which one CSS class on their box can do.

**State → page.** `hub_show_timer_page` picks the page from the state (planning or ended → plan, playing or paused → session, break → break) and is called after every state change, including once at the end of `hub_build_box`, the first moment the stack exists (calling it earlier, from `hub_timer_manager`, would pass a `NULL` stack).

**When a break comes.** `hub->timer.next_break` is the elapsed second of the next break: `break_min * 60` at Start. Each tick while playing, `hub_update_timer` checks `elapsed >= next_break` and switches to break. Breaks **count** toward the session (decided 17 Nov): the countdown keeps running in the break state, and the session can end during one. Done sets `next_break = elapsed + break_min * 60`, so every break is followed by a full `break_min` of play. To test without waiting: `break_every_minutes=1` in the settings file.

**Icons.** `gtk_image_new_from_icon_name` takes a name from the icon theme (Adwaita: `alarm-symbolic`, `input-gaming-symbolic`, `preferences-desktop-accessibility-symbolic`, `object-rotate-right-symbolic`, `weather-showers-symbolic`); `gtk_image_new_from_file` takes a file path, relative to the folder the hub was started from. The Adwaita icons are placeholders until custom ones arrive, which will come as a GResource (compiled into the program) or as installed named icons, not as relative paths.

**C or CSS.** Where things go is C (which boxes and pages, orientation, `gtk_widget_set_halign`, `gtk_box_set_homogeneous`, label wrapping); what they look like is CSS (colours, fonts, margins, borders). GTK CSS can't position widgets.

**The tick.** Since step 5 the tick is `g_timeout_add(1000, …)` instead of `g_timeout_add_seconds(1, …)`: the seconds version lines its ticks up with other programs' timers to save power and can fire almost a second late, so the countdown sometimes stayed on a number and then skipped one. Still once a second.

## Eye cue (milestone 3, step 6)

**What it does.** Every `eye_min` minutes of play (20), a line under the countdown, "Look at something 6m away for 20 seconds", shows for `eye_sec` seconds (20), then hides. No page switch, nothing to click: the 20-20-20 rule, quieter than the break.

**Showing and hiding.** `gtk_widget_set_visible(widget, FALSE)` hides a widget: it stays in its box but takes no space and isn't drawn. The label is created hidden in `hub_timer_manager`, and Start hides it again (a cue could still be showing from the last session).

**Two schedules, one idea.** Like `next_break`, `next_eye` is the elapsed second of the next cue (`eye_min * 60` at Start), and `eye_until` the second it hides. Each tick while playing, `hub_update_eye`: if `elapsed >= next_eye`, show it, `eye_until = elapsed + eye_sec`, and `next_eye += eye_min * 60` (*moved on by*, not *set to*: set to, it would fire on every tick after the first cue). If `elapsed >= eye_until`, hide it (hiding a hidden widget changes nothing, so this is harmless before the first cue). The eye check runs **after** the break check, so when both are due on the same second (minute 60 with 20/60) the break wins and no cue shows during it. To test: `eye_every_minutes=1`, `eye_seconds=5`.

**Text from the setting.** `hub_show_eye` builds the text with `g_strdup_printf` from `eye_sec`, so changing the setting changes the text too.

**Order in Start.** `hub_update_timer` is the last line of `hub_start_clicked`: it reads `next_eye`, so everything it uses has to be set first.

## Notifications and the bell (milestone 3, step 7)

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

## Plan-page choices (milestone 3, step 8)

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

## Milestone 3 wrap-up (19 Nov)

**Cost check** (session running, `top -b -n 3 -d 2 -w 200 -p (pidof gaming_hub)`): `%CPU` 0.0–0.5 %, `RES` 114 MB (0.7 % of RAM, of which ~84 MB `SHR`: GTK's shared libraries), 0.7 s of CPU in total. `top -b` cuts its lines at the terminal's width, hence `-w 200`. The full `glmark2` fps check comes after the styling step.

**Left for later:** the hub's own sound and icons (GResource or installed named icons, when the branding arrives); the `.desktop` file by hand until `make install` (milestone 4); styling (stats visible but not dominant; spin buttons; the break title).
