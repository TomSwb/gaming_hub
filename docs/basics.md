# Basics: app id, main loop, memory

How the program is put together (milestone 1) and who frees what. Back to the [README](../README.md).

## Application id

`ch.bitminded.GamingHub` (reverse of my domain `bitminded.ch`). It is only a name, nothing goes online: the system uses it to recognise the running app (single instance, milestone 4) and to name the `.desktop` file and the icon. **It never changes.** The window title, "Gaming Hub", can.

## How the program runs (milestone 1)

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

## Who frees what

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
