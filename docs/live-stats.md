# Live stats (milestone 2)

How the stats are read from `/proc` and `/sys`, and the fps-cost check. Back to the [README](../README.md).

**The tick.** `hub_activate` starts a timer with `g_timeout_add(1000, hub_tick, hub)` (until 17 Nov `g_timeout_add_seconds(1, …)`, see step 5): the main loop calls `hub_tick` once a second, and `hub_tick` returns `G_SOURCE_CONTINUE` to be called again (`G_SOURCE_REMOVE` would stop it). `hub_activate` also calls `hub_tick` once itself, so the labels are filled at once instead of after one second. Once a second is the fps rule: an interval of 0 would call it nonstop and eat CPU.

**Stopping the tick (fixed 24 Nov, milestone 7 step 6).** Until then the timer's id was thrown away, so nothing could stop it. Closing the window destroys the labels, but if one more tick fired before the app finished quitting, `hub_tick` wrote to five freed labels: five `invalid unclassed pointer in cast to 'GtkLabel'` `CRITICAL`s within ~10 ms, only now and then (a **race**: it depends on whether a tick lands in that gap, so a clean run proves nothing). Fix: `hub->tick_id` keeps what `g_timeout_add` returns (a `guint`, never 0), and the window's `destroy` signal (emitted while GTK tears the window down, before control returns to the main loop) calls `hub_window_destroyed`, which does `g_source_remove(hub->tick_id)` and sets it back to 0 (removing an id twice is a `CRITICAL` too). The handler is connected right after the window is made, before the timer starts. Checked with `G_DEBUG=fatal-criticals ./gaming_hub` (any `CRITICAL` aborts): clean on every close. The same risk exists in principle for the async callbacks (libpulse, BlueZ, power) that write labels; none seen so far.

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
