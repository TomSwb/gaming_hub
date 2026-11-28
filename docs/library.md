# The game library (milestone 7)

The hub's home screen: every game (Steam and my GameCube, Wii and PS2 dumps) in one list or grid, started from the hub. Built in 18 small steps; this page grows with them. Back to the [README](../README.md). Plan: [`game-dashboard.md`](https://github.com/TomSwb/set_up/blob/main/docs/gaming-hub/game-dashboard.md) and [`game-library.md`](https://github.com/TomSwb/set_up/blob/main/docs/gaming-hub/game-library.md) in my set_up repo.

## The steps (planned 22 Nov)

| # | Step | Estimate |
| --- | --- | --- |
| 1 | Look by hand (Steam files, ROM folders, a disc header, cover caches, libraries, emulator flags) | ~0.5 h |
| 2 | Game GObject (`HubGame`) | 3–4 h |
| 3 | Library page + list view with a few made-up games | 2–3 h |
| 4–5 | KeyValues parser, Steam source | 4–6 h |
| 6 | Emulator source: platforms file, ROM folders by extension | 3–4 h |
| 7 | Names from file names, disc header check | 1–2 h (done 24 Nov, ~1.2 h) |
| 8 | Launch and the session loop (`steam://`, emulators watched as a process); rescan when the library page is shown + a Refresh button (added 23 Nov; live folder watching is v2) | 4.5–7 h (done 27 Nov, ~5.3 h: 24 Nov ~2.4 h, 27 Nov ~2.9 h) |
| 9 | The hub's library file (`~/.local/share/gaming_hub/library.ini`) | 2–3 h (done 28 Nov, ~2.2 h) |
| 10 | Three states, live drives | 2–3 h (done 28 Nov, ~2.8 h) |
| 11 | Silent session log + play history | 3–5 h |
| 12 | Grid + toggle | 2 h |
| 13 | Search, filters, sort | 2–3 h |
| 14 | Hide + Show hidden | 1–2 h |
| 15 | Add manually | 2 h |
| 16 | Grouping, Edit game dialog, group-by | 4–7 h |
| 17 | Covers | 2–3 h |
| 18 | Wrap-up: fps check (`glmark2`), norm, CPU / RAM | ~0.5 h |

## Step 1: look by hand (22 Nov, ~0.3 h)

### Steam

**Library folders:** `~/.local/share/Steam/steamapps/libraryfolders.vdf`, in Valve's **KeyValues** format: a quoted key followed by a quoted value or by a `{ … }` block, blocks inside blocks, whitespace (tabs) between tokens, nothing else.

```
"libraryfolders"
{
	"0"
	{
		"path"		"/home/tomswb/.local/share/Steam"
		"apps"
		{
			"228980"		"300308402"
		}
	}
	"1"
	{
		"path"		"/mnt/Steam2TB/SteamLibrary"
		...
```

- One numbered block per library; the hub only needs its `"path"`. Mine: the SSD (6 apps, all Steam tools), Steam2TB (41 games), and an empty `/var/tmp/SteamLibrary` (on the SSD, no apps). **Rule:** a library that is empty or whose folder is missing is skipped without an error.
- `<path>/steamapps/` holds one `appmanifest_<appid>.acf` per app listed in `"apps"` (checked: 6 + 41).

**A manifest** (`appmanifest_400.acf`, Portal) is one `"AppState"` block. What the hub uses, all at its first level:

| Key | Example | Use |
| --- | --- | --- |
| `appid` | `400` | `steam://rungameid/400` |
| `name` | `Portal` | the name; **trim it** (`"Sonic Adventure™ 2 "` ends with a space); `™` is UTF-8, fine for GTK |
| `installdir` | `Portal` | the game's folder: `<library path>/steamapps/common/<installdir>` (states, step 10) |
| `StateFlags` | `4` | bit flags, 4 = fully installed |
| `LastPlayed` | `1795264178` | Steam's last-played time (Unix seconds, `0` = never); possible bonus for the play history |
| `LastOwner` | `76561198310493486` or `0` | used to skip tools (below) |

The blocks inside (`InstalledDepots`, `UserConfig`, `MountedConfig`) are skipped, so the parser must skip a whole block, nested `{ }` included.

**Tools are not games (decided 22 Nov, option A):** no manifest key says "tool". `LastOwner` is `"0"` for the Steam Linux Runtimes (1.0, 2.0, 4.0) and Proton 11.0, but my account id for Steamworks Common Redistributables (`228980`) and Ubisoft Connect Services (`4099820`, needed by the Assassin's Creed and Splinter Cell games). **Rule:** skip a manifest when `LastOwner` is `"0"` **or** its `appid` is in a short skip list in `defaults.h`. New Proton and runtime versions are caught by the first test. Rejected: a skip list only (every new Proton needs an entry), showing everything and hiding by hand (Hide is only for Not installed games). The only file that really says "tool", `appcache/appinfo.vdf`, is binary.

### My dumps

`~/Emulation/roms/gc` (4), `roms/wii` (6), `roms/ps2` (12): 22 games, all `.iso`, all named `Title (Region) [ID].iso`. Cases the name parser meets:

| Case | Example |
| --- | --- |
| Region with a comma | `Jak X (Europe, Australia) [SCES-53286].iso` |
| ` - ` subtitle, `&`, `.` | `Dragon Quest VIII - The Journey of the Cursed King`, `Asterix & Obelix XXL`, `New Super Mario Bros. Wii` |
| Title starting with digits | `007 - Nightfire` (sorts first A–Z) |
| ID with a dash | `[SLES-50539]` (PS2 serial), `[GLZF69]` (GameCube / Wii game ID) |

Read from the end: the last `[…]` is the ID, the `(…)` just before it is the region, everything before that is the title.

### The GameCube / Wii disc header

`xxd -l 128 <file>` on one disc of each:

| Offset | GameCube (`GLZF69`) | Wii (`RMGE01`) |
| --- | --- | --- |
| `0x00`, 6 bytes | game ID `GLZF69` | game ID `RMGE01` |
| `0x18`, 4 bytes | `00000000` | **`5D1C9EA3`** (Wii magic word) |
| `0x1C`, 4 bytes | **`C2339F3D`** (GameCube magic word) | `00000000` |
| `0x20` | `From Russia With Love` + `00` bytes | `SUPER MARIO GALAXY` + `00` bytes |

The header titles are worse than my file names: English on a French disc (and no `007 -`), all capitals on the Wii. **Decided 22 Nov (option A):** the **file name is the name source for every platform**; for GameCube / Wii the header is read as a check (the magic word says it's a real disc and which console) and its 6-byte ID is the fallback when a file name doesn't fit the pattern. Rejected: header titles (capitals, wrong language), or no header read at all.

### Covers

| Source | File | Size | Found |
| --- | --- | --- | --- |
| Steam | `~/.local/share/Steam/appcache/librarycache/<appid>/library_600x900.jpg`, else `library_capsule.jpg`, either directly in the folder **or one hash-named subfolder down** (Steam's newer layout) | 600 × 900 or 300 × 450 (2:3) | 41 / 41 |
| Dolphin | `~/.var/app/org.DolphinEmu.dolphin-emu/cache/dolphin-emu/GameCovers/<ID>.png` | 160 × 224 (5:7) | 8 / 10 (no GameTDB cover for the French `GLZF69`, `GSMF52`) |
| PCSX2 | `~/.var/app/net.pcsx2.PCSX2/config/PCSX2/covers/<serial>.jpg` | varies | 12 / 12 |

- Steam's other images: `header.jpg` / `library_header.jpg` (wide banner), `library_hero*.jpg` (backgrounds), `logo.png`, and a hash-named `.jpg` copy. Not used: the banner would need cropping.
- Dolphin downloads covers only with **Config → Interface → "Download Game Covers from GameTDB"** (switched on 22 Nov; the folder was empty before) and fetches them when the game list shows.
- PCSX2 has no source of its own: **Tools → Cover Downloader** with the community collection's address `https://raw.githubusercontent.com/xlenore/ps2-covers/main/covers/default/${serial}.jpg`, "Use Title File Names" left **unticked** (PCSX2 2.8 names covers by serial by default). Done 22 Nov.
- **Tiles 150–160 px wide** (decided 22 Nov), each cover fitted inside, not stretched (the sources have different shapes). No cover → the simple tile.

### Libraries and emulator flags

- `pkg-config --modversion`: **gtk4 4.22.5** (section headers need 4.12+; whether GtkGridView has them too: check at step 16), **gdk-pixbuf-2.0 2.44.4**, **gio-unix-2.0 2.88.4**. Nothing to install; json-glib only when Heroic comes.
- **Dolphin** (`--help`): `-e <file>` loads the file, `-b` runs without its window and needs `-e`. So `-b -e %f`.
- **PCSX2 2.8.2** (`-help`, one dash; `--help` prints nothing and exits 1): `-batch` exits when the game shuts down, `-fullscreen`, `--` ends the options so a file name with spaces stays one. So `-batch -fullscreen -- %f`. (`-nogui` would also hide the main window.)

## Step 2: the Game GObject (23 Nov, ~2.7 h)

`HubGame` is the shape of one game: the common format between the sources (Steam, ROMs, later Heroic) and GTK's list and grid. It has to be a GObject because GTK's model-based views (`GListStore`, `GtkListView` / `GtkGridView`) only hold GObjects; in return GTK gives scrolling, widget reuse (only visible tiles exist), section headers for the grouping, filters and sorting. Compare: a SQL table's schema (the struct), its rows (one object per game), its column names (the properties).

**Registered by hand (decided 23 Nov, option B).** The usual macros `G_DECLARE_FINAL_TYPE` / `G_DEFINE_TYPE` are calls outside any function, which norminette can't parse (it then checks nothing in the file), and `#define HUB_TYPE_GAME (hub_game_get_type())` fails its `PREPROC_CONSTANT` rule. So the hub writes what the macros would generate, with GLib's public functions. Rejected: the macros in two files kept as a documented norm exception (A), norminette skipped for the library (C).

| Piece | What | Where |
| --- | --- | --- |
| Types | `t_game_source` (`GAME_STEAM`, `GAME_EMULATOR`, `GAME_MANUAL`), `t_game_prop` (`GAME_PROP_NONE` = 0, unused, then one id per property, `GAME_PROP_COUNT`), `t_hub_game` (`GObject parent` **by value and first**, then the fields), `t_hub_game_class` (`GObjectClass parent_class`). The norm forbids struct declarations in a `.c` file, so the fields are public: no getters needed | `structs.h` |
| `hub_game_get_type` | `g_type_register_static_simple(G_TYPE_OBJECT, "HubGame", sizeof class, class_init, sizeof instance, init, 0)` inside `if (g_once_init_enter(&type_id))` … `g_once_init_leave(&type_id, result)`; `type_id` is a `static gsize` (zero by itself; norminette accepts it). The number changes every run, the name doesn't | `hub_game.c` |
| `class_init`, `init` | Written with the exact signatures GLib calls (`gpointer klass, gpointer data` / `GTypeInstance *instance, gpointer klass`), so they're passed **without casts** and the compiler checks them | `hub_game.c` |
| `finalize` | Frees the four strings, then **chains up**: `g_type_class_peek_parent(G_OBJECT_GET_CLASS(object))` gives GObject's class, whose `finalize` frees the object itself (no stored parent pointer: the norm forbids globals) | `hub_game.c` |
| Properties | `name`, `platform`, `id`, `path` (`g_param_spec_string`), `source` (`g_param_spec_int`, `GAME_STEAM`–`GAME_MANUAL`, default `GAME_MANUAL`: an enum property would need the enum registered as a type too), flags `G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS`, installed at once with `g_object_class_install_properties`. A writable property needs `set_property` on the class **before** install (GLib's `CRITICAL` otherwise) | `hub_game_props.c` |
| `set_property` / `get_property` | `if` / `else if` on `prop_id` (the norm forbids `switch`); strings stored as copies with `hub_keep`, read out with `g_value_set_string` (copies); unknown ids → `G_OBJECT_WARN_INVALID_PROPERTY_ID` | `hub_game_props.c` |
| `hub_game_cast` | `G_TYPE_CHECK_INSTANCE_CAST(object, hub_game_get_type(), t_hub_game)`: a wrong object gives a `CRITICAL` instead of silent garbage | `hub_game_props.c` |
| `hub_game_new` | **Decided 23 Nov (option C):** takes a `t_game_info *` (the five values, strings `const` and only lent) because the norm allows 4 parameters; one `g_object_new` with the five property pairs ending with `NULL`, so `set_property` stays the one place that stores values and the variadic call (unchecked by the compiler) exists only here. The caller gets one reference. Rejected: `g_object_new` with pairs in every parser (mistakes only at runtime), 4 parameters + `g_object_set` | `hub_game.c` |

`hub_mic_keep` became `hub_keep` and moved to `utils.c`: it now stores the games' strings too.

**Test** (`hub_test` in `main.c`, kept but not called): Portal from a `t_game_info` → `hub_game_new` → name read back with `g_object_get` (a copy: freed), platform / id / path read through `hub_game_cast`, `g_object_unref` → `finalize` ran once. Bugs on the way: the two sizes swapped (instance size given as an enum's: memory corruption at runtime), `class_init` **calling** `finalize` instead of storing it, casts on a line of their own (result thrown away; one stopped the build: "statement with no effect"), `g_free(&field)` / `&field = …`, the borrowed string from `g_value_get_string` stored without a copy, `prop_id` confused with `game->id`, a source enum stored as a pointer.

Code `gaming_hub` `1722035` … `7550de8`.

## Step 3: the library page and the list view (23 Nov, ~1.1 h)

The library page is the first page of the stack: a scrolling list with one row per game (name on the left, platform on the right), filled with three made-up games until the Steam source arrives.

**The chain.** A GTK list is two inputs joined in a view: the **data** side (`GListStore` of `HubGame`s → `GtkSingleSelection`, which only adds "which row is picked") and the **drawing** side (`GtkSignalListItemFactory`, the row recipe). `GtkListView` asks the selection how many games there are, works out which rows fit on screen, and asks the factory to build and fill only those; a `GtkScrolledWindow` around it shows one part and tells the view which part is visible. Compare: the store is a table, the selection the highlighted row, the factory the template for one row of a web page, the view the page that renders only what fits, the scrolled window the browser window. The grid (step 12) will be a second view on the same selection with another factory.

**The factory's life cycle.** `setup` builds an empty row once per row widget (only enough to fill the screen); `bind` fills a row with one game, again each time scrolling reuses that row for another game. `unbind` / `teardown` undo what `bind` / `setup` did; not needed here (labels are just overwritten). The order of the two `g_signal_connect` calls doesn't matter: GTK fires the signals, always `setup` before `bind`.

| Piece | What | Where |
| --- | --- | --- |
| `t_library` | `GListStore *games`, **borrowed** (the selection owns the store), so `hub_free_hub` doesn't unref it; set to `NULL` by `hub_library_init`. Will grow (running game, volume monitor, inner stack for list / grid, filters) | `structs.h`, `inits_2.c` |
| `hub_library_add` | `hub_game_new` (1 ref, mine) → `g_list_store_append` (transfer none: the store adds its own, 2) → `g_object_unref` (1, the store's). Used later by every source | `library_store.c` |
| `hub_library_fake` | Portal / Steam, Super Mario Galaxy / Wii, Jak X / PS2 from one reused `t_game_info` (it only lends; `set_property` copies). Deleted at step 5 | `library_store.c` |
| `hub_row_setup` | The empty row: `hub_labelled(NULL, gtk_label_new(NULL))` (expanding name label + platform label), handed to the row with `gtk_list_item_set_child` (which takes the floating reference) | `library_rows.c` |
| `hub_row_bind` | `gtk_list_item_get_item` → `hub_game_cast`, `gtk_list_item_get_child`, first / last child of the row → `gtk_label_set_text` with name / platform. Everything borrowed, nothing freed; `gtk_label_set_text` refuses `NULL` (remember for manual entries, step 15) | `library_rows.c` |
| `hub_build_library` | `g_list_store_new(hub_game_get_type())` → fake games → `gtk_single_selection_new` (**transfer full**: takes the store) → factory + both signals → `gtk_list_view_new` (**transfer full**: takes the selection and the factory) → scrolled window (`vexpand`). No unref anywhere: each object belongs to the next, the stack takes the page. 5 locals, the norm's maximum | `library_build.c` |
| Stack | `"library"` added first in `hub_build_box`; **decided (B):** shown at startup until step 8 wires "pick a game → plan" (the plan page is unreachable meanwhile; I'm not playing until the library is done). To get the plan back early, delete that line: the norm forbids comments inside a function | `main_build.c` |

`GtkSingleSelection` autoselects the first row (Portal is highlighted at start): kept, for "Enter starts the highlighted game" later. Arrow keys and clicks move the highlight. Screenshot: `assets/historic/library_done/first_test_rows.png`.

Bugs on the way: the `setup` row never handed to the list item (empty row and a leaked floating widget) inside an extra box, a variable named `label` holding a box, a space before `)` and a split right after `(`, the `bind` signal not connected, `library_rows.c` missing from the Makefile (the clean build hadn't compiled it), header prototypes misaligned.

Code `gaming_hub` `e117fa0` … `96731cd`.

## Step 4: the KeyValues parser (23 Nov, ~1.3 h)

A reader for Valve's KeyValues text (`libraryfolders.vdf`, `appmanifest_*.acf`), written by hand like a 42 parser: one buffer, one walking pointer. GLib only for the boring parts (`g_ascii_isspace`, `g_strndup`; step 5 reads the file with `g_file_get_contents`, the libft equivalents would be `ft_substr` and a `read` loop). Allocated with `g_`, freed with `g_free`.

**Decided 23 Nov (option B): walk the text, take what's needed.** No tree: a cursor walks the buffer, and the two file shapes of step 5 get their own small loops on top of "enter a block by its key" and "skip a block". The only owned things are the file buffer and the strings kept. Rejected: a tree of nodes queried afterwards (A: general, but a node struct, a recursive build and a recursive free, 8–10 functions, power not needed: Heroic will be JSON), a callback parser (C: function pointers, a state struct, "where am I" tracked by depth numbers).

| Piece | What | Where |
| --- | --- | --- |
| `t_kv_token` | `KV_STRING`, `KV_OPEN` (`{`), `KV_CLOSE` (`}`), `KV_END` (the `'\0'`), `KV_ERROR` (stray character, unclosed string). A string is a key or a value: only its place tells | `structs.h` |
| `t_kv_cursor` | `pos` (the walking pointer, `const`: reads only, borrows the buffer) and `line` (counted at every `'\n'`, for error messages). A struct so new state doesn't change every prototype | `structs.h` |
| `hub_kv_skip_space` | steps over whitespace (`g_ascii_isspace`), counting lines; `'\0'` stops it by itself | `keyvalues.c` |
| `hub_kv_read_string` | on a `"` (else `KV_ERROR`): walks to the closing `"` or `'\0'`; a `\` steps over the next character so `\"` doesn't end the string; **escapes kept as they are** (decided: never in my Linux files, unescaping would mean a char-by-char copy); unclosed → `KV_ERROR`, nothing copied; else `*text = g_strndup(start, pos - start)`, owned by the caller. Spaces inside quotes are text: the trailing space of `"Sonic Adventure™ 2 "` is trimmed in step 5, for the name only | `keyvalues.c` |
| `hub_kv_next` | the tokenizer: `*text = NULL` first (so the caller can always `g_free` it), skip space, then `'\0'` → end (no move), `{` / `}` (one step), `"` → `read_string`, else error (no move, so `line` points at the bad character) | `keyvalues.c` |
| `hub_kv_skip_block` | called just after a `KV_OPEN`: depth from 1, +1 / −1 per brace, every string freed at the bottom of the loop, `FALSE` on end or error before the matching `}` | `keyvalues.c` |
| `hub_kv_enter` | reads key-value pairs at one level until the key asked for has a block: `TRUE` with the cursor just inside its `{`. Other blocks skipped whole, plain pairs passed, compare **before** freeing. `FALSE` at the level's `}`, the end, or a broken file | `keyvalues.c` |

**Test** (`hub_test`, not called): a made-up manifest (an `appid`, a two-level `InstalledDepots` block, a name with a trailing space) as four joined string literals. `enter("AppState")` → 1, line 2; then a key not there → 0 at line 12 (the level's `}`: walked one level, skipped the nested block whole); with the name's closing `"` removed → 0 at line 13 (unclosed string ran to the end: an error, told apart by the line); `enter("InstalledDepots")` → 1, line 5 (just inside its `{`). Lesson: **C doesn't fix the order in which a function's arguments are evaluated**: `g_print(…, hub_kv_enter(&cursor, …), cursor.line)` printed the line from *before* the call; store the result first. Removing only the `"` of a `\"` in C source leaves a `\` that escapes the next `n`.

Bugs on the way: `||` instead of `&&` (walked past the end of the buffer), `*cursor->pos + 1` (the character's value + 1, not the next character), the line counter moved instead of the cursor, a bare `{ }` block, `g_free(text)` on the address of the caller's variable, every string treated as an error in `skip_block`, then a double free, `name` compared with its own value, `hub_kv_skip_block` called in the conditions instead of testing the token, `hub_test` never called, two hubs left paused with Ctrl+Z (`fg` then Ctrl+C).

Code `gaming_hub` `b5934af`.

## Step 5: the Steam source (23 Nov, ~2.1 h)

The hub reads my real Steam library: `libraryfolders.vdf` → each library's `steamapps/` → every `appmanifest_*.acf` → the tools skipped → the game in the store. **Result on my laptop: 3 libraries, 47 manifests (6 on the SSD, 41 on Steam2TB, 0 in `/var/tmp`), 6 tools skipped, 41 games, no error line.** The made-up games (`hub_library_fake`) are deleted. Names keep their UTF-8 (`Mass Effect™ Legendary Edition`) and lose trailing spaces; the order is the order the folders were walked (sorting: step 13).

**The chain** (each level frees what it made before returning):

```
hub_steam_scan(store)                     ~/.local/share/Steam/steamapps/libraryfolders.vdf
└─ hub_kv_enter "libraryfolders"
   └─ hub_steam_folders                   each numbered block (plain pairs passed over)
      ├─ hub_steam_lib_path               that block's "path" (its "apps" block skipped)
      └─ hub_steam_library(store, path)   GDir on <path>/steamapps (missing folder: skipped quietly)
         └─ hub_steam_app(store, folder, file name)   each appmanifest_*.acf
            ├─ hub_kv_enter "AppState" + hub_steam_app_read → t_steam_app
            ├─ hub_steam_app_skip         missing id / name / installdir, LastOwner "0", or id in the skip list
            ├─ hub_steam_app_add          trimmed name, <folder>/common/<installdir>, t_game_info → hub_library_add
            └─ hub_steam_app_free
```

| Piece | What | Where |
| --- | --- | --- |
| `hub_kv_load` | `g_file_get_contents` (the whole file, `'\0'`-ended) and the cursor pointed at it; the **caller owns the buffer** and frees it after the cursor is done. `GError *error = NULL` first (GLib's rule); on failure the message is printed and `NULL` returned | `keyvalues_2.c` |
| `hub_kv_pair` | one key and its value: `KV_STRING` (both owned), `KV_OPEN` (key owned, value `NULL`), `KV_CLOSE` / `KV_ERROR` (nothing to free: an orphaned key is freed and cleared, `*name = NULL`) | `keyvalues_2.c` |
| `hub_kv_take` | **moves** a string instead of copying it: frees the slot's old string, the slot takes the pointer, the source becomes `NULL` (so the usual free at the bottom of a loop frees nothing) | `keyvalues_2.c` |
| The level loop | read a pair before the loop; each turn: use it, free `name` / `value`, then **either** skip the block (`KV_ERROR` if that fails) **or** read the next pair; ends on `KV_CLOSE` (clean) or `KV_ERROR`. Used by `hub_steam_lib_path`, `hub_steam_folders`, `hub_steam_app_read` | `steam.c`, `steam_app.c` |
| `hub_steam_scan` | `g_build_filename(g_get_home_dir(), …, NULL)` (owned; the home dir borrowed), load, enter, folders; a broken file prints `file: line N: not a valid Steam library list`; the cursor lives on the stack, lent with `&` | `steam.c` |
| `hub_steam_library` | `g_dir_open` (`NULL` = missing library, not an error), `g_dir_read_name` read before the loop and at the bottom (names borrowed from the `GDir`), `g_str_has_prefix` / `_suffix` on the **name** | `steam.c` |
| `t_steam_app` | `id`, `name`, `dir`, `owner`: **owned** strings (the opposite of `t_game_info`, which lends), cleared with `ft_bzero`, filled by `hub_steam_app_keep` (one `hub_kv_take` per wanted key), freed by `hub_steam_app_free` | `structs.h`, `steam_app.c`, `steam.c` |
| Skip list | `HUB_STEAM_SKIP "228980;4099820"`: `g_strsplit` → a strv, `g_strv_contains` (needs the cast `(const gchar *const *)`: C won't add that `const` by itself), `g_strfreev` frees the array and its strings. Cheap checks first, the list freed before returning | `steam_app.c`, `defaults.h` |
| `hub_steam_app_add` | `g_strstrip` trims **in place** (the name is my own writable copy), the install path built and freed, the info lends, the game copies | `steam_app.c` |

Defines for every outside name (`defaults.h`, `HUB_STEAM_*`): one place for Valve's spellings, the skip list and the paths (the libraryfolders path is the one v2 changes for Flatpak Steam).

Bugs on the way: `GError` not set to `NULL`, `g_free(name)` / `name = NULL` on the address instead of the string, a failed skip overwritten by the next read, a comma operator in a `return`, `KV_STRING` tested instead of `KV_OPEN`, `while (dir)` (never ends) with the read result thrown away, prefix / suffix tested on the folder instead of the entry, a built path leaked into `g_print`, 27 lines (the per-manifest work moved into `hub_steam_app`), a field assigned then freed (dangling) and not cleared in the caller (double free), `g_free` on the caller's variable's address, the `"path"` test left over in the manifest loop, the skip returns flipped, `hub_kv_skip_block` used for `hub_steam_app_skip` and the read never called.

Code `gaming_hub` `8648ebc`, `ddb6d3a`.

## Step 6: the emulator source (23–24 Nov, ~1.2 h)

The hub reads my emulator games from a **platforms file** next to Steam: one section per platform, every file in its folder with one of its extensions becomes a game. **Result: 66 rows** (41 Steam + 25 dumps: 4 GameCube, 6 Wii, 12 PS2, and the 3 Metroid Prime discs in their own PrimeHack folder), no terminal output. Names are still whole file names (`Sonic Heroes (Europe) [SLES-51950].iso`) until step 7; `command` and `icon` are read at step 8.

**Decided 23 Nov (option A):** the file is `~/.config/gaming_hub/platforms.ini`, next to `settings.ini`. Missing = no emulator games and no message (same rule as the settings file, through the same `hub_key_file_error`); a section without `folder=` or `extensions=` gives one line and is skipped. The repo has a commented [`platforms.example.ini`](../platforms.example.ini) (README: "Emulator games"). Rejected: a default file written by the hub (B: my emulators would be named in the code), sections inside `settings.ini` (C: timer values and consoles mixed).

**Found 24 Nov:** the three Metroids aren't in `roms/` but in `~/Emulation/ports/primehack` (one folder, GameCube and Wii mixed), so they get a **`[PrimeHack]` section** with PrimeHack's command; for now their platform shows as "PrimeHack" (the section name); step 7's header check could give GameCube / Wii instead. **Jak 1–3 (OpenGOAL) stay out:** OpenGOAL extracted the ISOs once and runs its own build, there is no file to hand an emulator; they become manual entries (step 15).

**Decided 24 Nov (option A): a leading `~/` in `folder=` is expanded by the hub** (GKeyFile reads text only; `~` is a shell feature), so the example works for anyone. Rejected: full paths only (B: a copied `~` path would silently show nothing).

```
hub_build_library
├─ hub_steam_scan(store)
└─ hub_platforms_scan(store)              ~/.config/gaming_hub/platforms.ini (missing: quiet)
   └─ hub_platform_read(store, key_file, group)     each section
      ├─ hub_path_expand(&folder)         ~/… → /home/<me>/…
      └─ hub_platform_folder(store, group, folder, extensions)   GDir (missing folder: quiet)
         ├─ hub_platform_ext_ok(name, extensions)
         └─ hub_platform_add(store, group, folder, name)          → hub_library_add
```

| Piece | What | Where |
| --- | --- | --- |
| `hub_platforms_scan` | builds the path (`g_get_user_config_dir`, `HUB_CONFIG_DIR`, `HUB_PLATFORMS_FILE`), loads with `hub_key_file_error` (reused from the timer: `TRUE` = loaded), `g_key_file_get_groups` (**all** section names at once in an owned strv; the second argument is an optional count output, not an index) → one `hub_platform_read` each → `g_strfreev`; key file and path freed on every path | `platforms.c` |
| `hub_platform_read` | `g_key_file_get_string` (folder, owned) and `g_key_file_get_string_list` (extensions split on `;`, owned strv), both `NULL` when missing (error argument `NULL`: the reason isn't needed); one error line naming the section, else expand + walk; both freed at the bottom (`g_free(NULL)` / `g_strfreev(NULL)` are safe) | `platforms.c` |
| `hub_platform_ext_ok` | the extension = after the **last** dot (`ft_strrchr`: `New Super Mario Bros. Wii … .iso`); no dot or a leading dot only (`.hidden`) → `FALSE`; `g_ascii_strcasecmp(…) == 0` against each extension (case ignored; `g_strv_contains` is case-sensitive). Nothing allocated: `dot` points into `name` | `platforms.c` |
| `hub_platform_folder` | `g_dir_open` (`NULL` = missing folder or unplugged drive: skipped, step 10 shows it), names borrowed from the `GDir` (valid until the next read: the game copies them first), not recursive | `platforms.c` |
| `hub_platform_add` | `t_game_info`: `GAME_EMULATOR`, name = file name, platform = section, id = `NULL`, path = `g_build_filename(folder, name)` (owned, freed after `hub_library_add`) | `platforms.c` (full at 5) |
| `hub_path_expand` | replaces an owned string **in place** (the `hub_keep` / `hub_kv_take` shape, the caller passes `&folder`): if it starts with `HUB_HOME_PREFIX` (`"~/"`), builds `home + rest` first, then frees the old one; `NULL` slot or string → no change. Reused at step 15 | `paths.c` |

**Tests:** the four section names printed; each section's folder and extensions; `folder=` renamed to `foldr=` in `[Wii]` → `platforms.ini: [Wii] needs folder= and extensions=` and the other three still read; the 25 file names; 66 rows in the window; the PS2 folder written as `~/Emulation/roms/ps2` → still 66; norminette clean.

**Fixed on the way (an old bug, milestone 2):** five `invalid unclassed pointer in cast to 'GtkLabel'` `CRITICAL`s, sometimes, when closing the window: the tick wrote to the five freed stat labels. Now the timer's id is kept and the window's `destroy` signal removes it (`docs/live-stats.md`, "Stopping the tick"). Found with `G_DEBUG=fatal-criticals` and by counting: five failures = the five labels `hub_tick` writes.

Bugs on the way: `g_build_filename` without its `NULL` end, `i` thought to be an argument of `get_groups`, `self` and `store` unused (`-Wunused-parameter` + `-Werror`), the window created twice (a second, lost window) instead of the `destroy` connection, a 110-column `g_printerr`, a `folder` named `file`, `g_ascii_strcasecmp` tested without `== 0` (every other extension "matched"), the `NULL` check on the slot (`path`) instead of the string (`*path`), two empty lines in a header.

Code `gaming_hub` `b40cada`, `b5d0e79` (tick fix), `e88dd14`, `fea2d65`, `7ee68d4`.

## Step 7: names from the file names, the disc header check (24 Nov, ~1.2 h)

Every dump now shows a clean title (`Sonic Heroes`, `Jak X`, `Metroid Prime 2 - Echoes`) and carries its ID (`SLES-51950`, `RMGP01`) in the game's `id`. For GameCube / Wii discs the hub reads the first 32 bytes too: the ID from the disc fills in when the file name has none, and a name whose ID differs from its disc gives one warning line. All 13 GameCube / Wii dumps (incl. the Metroids) match.

**Decided 24 Nov:** (1B) the region is parsed but **not stored**: optional in the spec, nothing uses it yet, the GameCube / Wii ID encodes it; (2A) the platform **stays the section name**, also for the PrimeHack folder: step 8 launches a game with its section's `command`, so a header-made "Wii" would start Metroid in Dolphin; and the section name is the user's spelling; (3B) the header is read for **every** file (the magic word decides; PS2 files have none), as fallback **and** check; on a mismatch the **file name's ID is kept** (the name is the one source of truth) and the warning says to fix it.

```
hub_platform_add(store, group, folder, name)
├─ hub_name_parse(name, &dump)          t_dump_name: title always, region + id if the pattern fits
│  └─ hub_name_split(name, len, &dump)  from the end: [ID], then (Region), then the title
├─ g_build_filename(folder, name)       path
├─ hub_disc_check(path, &dump)          GameCube / Wii only
│  └─ hub_disc_id(path, id)             32 bytes, magic word, 6-byte ID into the caller's buffer
├─ hub_library_add (title, id, section, path)
└─ hub_name_clear(&dump), g_free(path)
```

| Piece | What | Where |
| --- | --- | --- |
| `t_dump_name` | `title`, `region`, `id`: owned strings, together because one name gives three results; `ft_bzero` first (garbage → `NULL`s, nothing freed), `hub_name_clear` last (frees, back to `NULL`, safe twice) | `structs.h`, `names.c` |
| `hub_name_split` | searches **backward** only, each search limited to what's before the last find (`g_strrstr_len`): the stem must end in `]` → the last `[` → `") "` right before it → the last `(` before that `)`; slices by pointer subtraction (`g_strndup(start, end - start)`), title trimmed with `g_strstrip`. Returns `FALSE` before any allocation when the pattern doesn't fit | `names.c` |
| `hub_name_parse` | the entry point: stem length (last dot, same rule as `hub_platform_ext_ok`), `split`, else the whole stem as title. **The title is always set** | `names.c` |
| `hub_disc_id` | `open` / `read` 32 bytes / `close` at once (`<fcntl.h>` for `open`: libft only brings `<unistd.h>`); `got != 32` covers short, empty and failed reads (`ssize_t`: `-1` fits); Wii word `5D 1C 9E A3` at 24, GameCube `C2 33 9F 3D` at 28, compared with `ft_memcmp` (string functions stop at `0x00`), as `\x` string defines; the ID copied into the **caller's** 7-byte buffer (no allocation) | `disc.c` |
| `hub_disc_check` | not a disc → nothing; no ID in the name → `g_strdup` the disc's (the buffer dies with the function, `dump` must own); different → one line on stderr | `disc.c` |

**Tests:** the 25 titles clean in the list; two empty test files: `Odd name.iso` → `Odd name`, `No region [TEST-1].iso` → kept whole (no `(Region)`); silent with the check wired (13 GameCube / Wii names match their discs); a 32-byte fake (`head -c 32` of Galaxy 2) named `Fake (Europe) [RMGP01].iso` → `…: the name says [RMGP01], the disc says [SB4P01]`; test files removed; norminette clean.

Bugs on the way: locals named `open` / `close` (they hide the POSIX functions piece 4 needed: renamed `bracket`, `paren_end`), `false` for `FALSE`, `open` undeclared (`<fcntl.h>`), the magic words compared at offset 0 and with the logic inverted (`!ft_memcmp` = a match), then against the empty `id` buffer, `close(-1)` when `open` failed, the title printed as the disc's ID and no `\n`, the check written but not called (a silent run proved nothing until it was wired).

Code `gaming_hub` `936239b`, `7679a46`.

## Step 8: launch and the session loop (24 + 27 Nov, ~5.3 h)

From the library to a running game and back. Split into 9 pieces: 1–4 done 24 Nov, 5–9 done 27 Nov. The whole loop now runs: library → pick → plan page → Start → game (Steam or emulator) → game closes → library with the "Between Games" bar → the next game at once → … → End Session.

| # | Piece | State |
| --- | --- | --- |
| 1 | Look by hand: PrimeHack's flags, an emulator and a `steam://` link from the terminal | done (~0.3 h) |
| 2 | Decisions: where the command comes from, what a pick does, how the hub's own launches fit `--start` / `--end` | done (~0.1 h) |
| 3 | Pick a game: `activate`, the picked game, plan page with Back and "Game:", pages that follow the state, sections hidden on the library | done (~1.25 h) |
| 4 | Launch a Steam game from Start, "Starting … / Playing …" on the session page | done (~0.5 h) |
| 5 | Launch an emulator game: `command` on the game (`t_platform`), argv with `%f` as one element, `gamemoderun` in front, `GSubprocess` | done (~1.3 h) |
| 6 | Watch the process: `g_subprocess_wait_async`, exit code, then `hub_game_closed` | done (~0.3 h) |
| 7 | Between games = the library with the timer and End session on top; a pick launches at once | done (~0.8 h, with the GameMode `--start` investigation) |
| 8 | Rescan when the library page is shown + a Refresh button | done (~0.3 h) |
| 9 | Tests (Steam, Dolphin, PCSX2, PrimeHack, switching games, End session), norm, ownership | done (~0.2 h) |

### Piece 1: by hand

| Launch | Returns | The hub learns the game closed from |
| --- | --- | --- |
| `steam://rungameid/<id>` (`gio open`, the same lookup as `g_app_info_launch_default_for_uri`) | **at once**, before the game (or even Steam) has started | GameMode's `end=` hook (`--end`) only |
| `gamemoderun flatpak run io.github.shiiion.primehack -b -e <file>` | when the game's window closes: `0`; at once with `1` when the file doesn't exist | the process ending (and `--end` from GameMode too) |

- **PrimeHack** (`--help`, a Dolphin fork, prints `Usage: dolphin-emu`): `-e <file>` and `-b, --batch` ("Requires --exec or --nand-title"), so `-b -e %f` is right, like Dolphin.
- GameMode's `start=` hook fires for these launches too (the hub opened), so the hub will get `--start` / `--end` for its **own** launches.
- `gamemoderun flatpak run …` prints GameMode `ERROR` lines about the portal (`bwrapinfo.json: empty file` when registering, `/proc/pid/…: No such file` when unregistering), also with a real game; GameMode still switched on (the hook ran). Looks like the preloaded library registering again from processes further down the Flatpak chain. Harmless, noted.
- Steam games: inside Steam's container the GameMode library isn't found for Portal (32-bit: `libgamemodeauto.so.0 … cannot be preloaded`, `dlopen failed - libgamemode.so`); GameMode's tweaks probably don't apply to 32-bit games (32-bit GameMode package?). Not the hub's problem; the hooks still fired.
- File names with spaces and brackets: in fish a list item (`set game …/*.iso; … $game[1]`) stays **one argument**, exactly what the hub's argv does with `%f`. Tab completion stops at the part all three Metroid files share.
- Starting a game while Steam isn't running starts Steam first (slow; Portal crashed once that way); the link still returns at once.

### Piece 2: decisions (24 Nov)

- **(2a) Option A: the command goes on the game.** `hub_platform_read` reads `command=` with `folder=`; every game gets a sixth property, `command`; a section's values travel together in a `t_platform` struct (passed by pointer, keeps `platforms.c` under 4 parameters). Launching needs only the game; manual entries (step 15) use the same field; a missing `command=` is found at scan time; `platform` stays free to become a display name later; `icon=` will travel the same way. Rejected: (B) read `platforms.ini` again at launch by the section name (ties launching to the name forever, no per-game command, icons impossible in `bind`), (C) a section → command `GHashTable` in `t_library` (same ties, more ownership).
- **(2b) Option A: double-click or Enter** (GTK's default `activate`; single click only highlights). Rejected: single-click activate (hover highlights rows, a stray click starts a game).

  | State | A pick… |
  | --- | --- |
  | No session (planning, ended) | keeps the game, shows the **plan page** with "Game: Portal" and **Back to library**; **Start** launches the game and starts the countdown together |
  | Between games | launches **at once** (the session is already planned; piece 7) |
  | Playing, paused, break | can't happen: the library isn't shown |

  Hub opened by GameMode (`--start`, a game started elsewhere) → plan page with no game; from the app menu → library.
- **(2c)** (1A) the state becomes **playing when the hub launches**; the `--start` that follows only updates the label. Rejected: (1B) wait for `--start` (a game without GameMode would never start the session). (2) Emulators: the process watch **and** `--end` both call `hub_game_closed`, which acts only once; the watch stays (works without GameMode, gives the exit code). (3) A launch that fails (an error from GLib) changes **nothing**, one error line; an emulator ending with a non-zero code counts as a close (between games) plus one error line.
- **The library page shows only the library** (decided 24 Nov): the Sound / power / stats sections show on every other page (plan, session, break, between games).
- Layout gap between the timer and the Sound section (the stack takes the leftover height because the library's scrolled window expands): left for the **styling step** (idea: plan centred under the Back row with `vexpand` + `valign` on the plan widget).

### Piece 3: pick a game

```
GtkListView "activate" (double-click / Enter)
└─ hub_library_activate            game at position → hub->library.picked, "Game: …", plan page
plan page = hub_build_plan_page    [Back to library]  Game: Portal   (GtkCenterBox)
│                                  └─ hub_build_plan (unchanged)
└─ Back → hub_library_back         picked dropped, library page
stack "notify::visible-child-name"
└─ hub_page_changed                hub->sections hidden on "library", shown elsewhere
```

| Piece | What | Where |
| --- | --- | --- |
| `t_library.picked` | `t_hub_game *`, **owns one reference**: the game the plan page will start, then the game being played (kept after the session for step 11's log); `NULL` = none. Dropped with `g_clear_object` (unref + `NULL` in one call, safe on `NULL`) at every new pick, Back, `--start` without a session, and exit. A plain pointer would dangle after the rescan (piece 8) empties the store | `structs.h`, `inits_2.c`, `utils.c` |
| `hub_library_activate` | the signal's fixed signature (`GtkListView *`, `guint position`, `gpointer`); `g_list_model_get_item` on the view's model (`gtk_list_view_get_model`: transfer none, nested) is **transfer full**, so the reference is **moved** into `picked` (no ref, no unref); `Game: %s` with `g_strdup_printf` (owned, freed after `gtk_label_set_text`, which copies) | `library_pick.c` |
| `hub_build_plan_page` | vertical box: a `GtkCenterBox` row (start = Back, center = the "Game:" label, exactly centred whatever the button's width), then `hub_build_plan(hub)`; `t_library.next` borrows the label. Replaces `hub_build_plan` as the stack's `"plan"` page | `library_pick.c`, `main_build.c` |
| `hub_library_back` | `(void)self`, drop `picked`, show `"library"` (`set_visible_child_name`: `add_named` happens once, at build) | `library_pick.c` |
| Pages | `hub_show_timer_page`: planning / ended → **`"library"`** (was `"plan"`); the forced `"library"` line in `hub_build_box` (the step-3 stop-gap) removed. `hub_game_started`: `--start` while planning or ended → `"plan"`, `picked` dropped and the label emptied (the hub doesn't know that game) | `show.c`, `main_build.c`, `game_events.c` |
| `hub->sections` | a vertical box holding Sound, power and stats, borrowed by `t_hub` like `stack` | `structs.h`, `inits.c`, `main_build.c` |
| `hub_page_changed` | `GObject::notify` handler on the stack's `visible-child-name` property: one line, `gtk_widget_set_visible(hub->sections, g_strcmp0(name, "library") != 0)` (`g_strcmp0` accepts the `NULL` name). Catches every page switch, wherever it comes from. **Order matters:** `hub->sections` is created and the handler connected **before** the first `add_named`, the only moment the library *becomes* visible (the later `hub_show_timer_page` sets the same page: no change, no notify) | `main_build.c` |

### Piece 4: launch a Steam game

| Piece | What | Where |
| --- | --- | --- |
| `hub_launch_steam` | `HUB_STEAM_RUN` `"steam://rungameid/%s"` + the id (owned, freed) → `g_app_info_launch_default_for_uri(uri, NULL, &error)`; `GError` from `NULL`, read and freed only on failure, one line on stderr | `launch.c` |
| `hub_launch` | the dispatcher, `TRUE` = go ahead: no picked game (`--start` case) → `TRUE`; Steam → its result; other games → `<name>: can't launch this game yet` and `FALSE` (until piece 5); on success `hub_show_playing(hub, HUB_STARTING)`. The `NULL` test comes first in the Steam test (`picked && picked->source == …`: `&&` stops before reading `source`) | `launch.c` |
| `hub_start_clicked` | `if (!hub_launch(hub)) return ;` first, so a failed launch changes nothing | `buttons.c` |
| `t_library.playing`, `hub_show_playing` | a label at the top of the session page (borrowed); the helper fills it with a format define and the picked game's name (`HUB_STARTING` `"Starting %s ..."`, `HUB_PLAYING` `"Playing %s"`), or empties it with no game. `Starting …` after the launch; `--start` while playing, paused or in a break (the game is really up) → `Playing …`. Steam can take a minute (Steam starting, Proton, shaders), so the label shows it's on its way | `timer_build.c`, `game_events.c`, `defaults.h` |

**Tests:** pick (double-click and Enter) → plan page; Back → library; start → library only, the sections on every other page; Portal → Start → `Starting Portal ...` → `Playing Portal` → quit → between games; a dump → Start → the error line, still on the plan page; `./gaming_hub --start` (hub closed) → plan page, empty label → Start → countdown only, no crash; norminette clean.

Bugs on the way: the handler connected without `G_CALLBACK` (incompatible pointer type), the plan page built but never added to the stack (`Child name 'plan' not found`), then the library's `add_named` deleted instead of the forced line, `add_named` used for Back (a second library and a full rescan), `(void)self` missing, the notify handler switching the **page** back to the library instead of hiding the sections (then still inside an `if`, so it never hid), the "no game" case without `return` (a `NULL` dereference), a define named `HUB_PLAYINGloading` (pushed code didn't build), the early return that skipped emptying the label, then `NULL->source` when it was removed, a test call that launched every pick (dumps included) through Steam, `next` / `sections` not set in the inits.

Code `gaming_hub` `5140213` … `cde637d`.

### Piece 5: launch an emulator game (27 Nov)

**Decided (5.2, A):** a section without `command=` is skipped with one line, like `folder=` / `extensions=` (`platforms.ini: [Wii] needs folder=, extensions= and command=`). Rejected (B): list its games and fail only at Start.

| Piece | What | Where |
| --- | --- | --- |
| `command` property | a sixth string on the game (owned copy, `NULL` for Steam): struct field, `GAME_PROP_COMMAND` before the count, `t_game_info.command` (borrowed), `HUB_GAME_COMMAND`, `init` / `finalize` / `hub_game_new`, install / set / get. **Trap:** `t_game_info` is filled field by field in `platforms.c` and `steam_app.c` without `ft_bzero`, so both must set `command` (a garbage pointer would be copied by `g_object_new`) | `structs.h`, `defaults.h`, `hub_game.c`, `hub_game_props.c`, `steam_app.c` |
| `t_platform` | a section's values in one struct: `name` (borrowed from the scan's group list), `folder`, `extensions`, `command` (owned by `hub_platform_read`, freed at its end; each game copies the command). Lent down as `const t_platform *`: `hub_platform_folder` 4 → 2 parameters, `hub_platform_add` 4 → 3 | `structs.h`, `platforms.c` |
| `hub_launch_argv` / `hub_launch_fill` | no shell: `g_shell_parse_argv` splits `command=` (from my own file) into words, a new `g_new0` array of `argc + 2` gets `gamemoderun`, every word copied, the word `%f` replaced by the game's path as **one** element, and the `NULL` end from `g_new0`. Copies everywhere, so `words` is freed whole (`g_strfreev`) and the caller owns `argv`. A shell string would re-read spaces, brackets, `'`, `$` or `;` in file names | `launch_argv.c` |
| `hub_launch_emulator` | argv → `g_subprocess_newv((const gchar * const *)argv, G_SUBPROCESS_FLAGS_NONE, &error)` (the const cast C needs), argv freed at once, the old process dropped, the new one **moved** into `t_library.running` (one owned reference; dropping it doesn't stop the program). `hub_launch`'s placeholder replaced by this branch | `launch.c` |

Checked by printing the argv as `[%s]` lines: all three platforms right, each path one element.

**PrimeHack's pad (not hub code, ~0.1 h):** PrimeHack is its own Flatpak with its own settings, so the Night Blossom mapping from Dolphin (16 Nov) wasn't there: port 1 was set to `SDL/0/Generic X-Box pad` but mapped to keyboard keys. Its `GCPadNew.ini` had no PrimeHack-only keys (a `diff` of the key names showed only version differences), so Dolphin's file was copied over it (backup `GCPadNew.ini.bak`). Metroid Prime 1 + 2 play with the original GameCube controls; Prime 3 (Wii, PrimeHack's Metroid Wii Remote controls) still to do.

### Piece 6: watch the process

`g_subprocess_wait_async(proc, NULL, hub_game_exited, hub)` right after the launch: returns at once, the main loop calls back when the emulator ends (the wait keeps its own reference until then). `hub_game_exited` (the fixed `GAsyncReadyCallback` signature): `g_subprocess_wait_finish`; exited with a code other than 0 → `emulator ended with code N` (still a close, decision 2c.3); then, **only if the process is still `hub->library.running`** (pointer comparison: a late old process must not close a newer game's session), drop `running` and `hub_game_closed`. `running` also freed in `hub_free_hub`. Quitting the hub while an emulator runs: no warnings, the game keeps running.

Found: Dolphin shows its own "file does not exist" dialog even with `-b`, and exits with 1 after OK. The process watch fires before GameMode's `UnregisterGame` lines, so it doesn't depend on GameMode.

### Piece 7: between games is the library

**Decided (7.1, A):** between games shows the library and the bar only; the Sound / power / stats sections stay hidden (they're back on the next game's session page). Rejected (B): sections there too (a much shorter list).

- `hub_build_transit` now builds a slim **bar** (`GtkCenterBox`: "Between Games" · the countdown · End Session, the two widgets that already existed), borrowed as `t_timer.transit_bar`, appended above the stack; the `"transit"` page is gone.
- `hub_show_timer_page`: no session **and** between games → `"library"`; one line `gtk_widget_set_visible(transit_bar, transit)` covers every state change.
- `hub_library_activate`: between games → `hub_launch`, and if it worked → `hub_game_started` (the milestone-4 between → playing transition: state, break rule, session page), then `return`; a failed launch stays in the library.

**Found: "Playing" sometimes never comes for Steam games.** `[cmd]` / `[pick]` prints (`g_printerr`: stdout is block-buffered in a pipe, so `g_print` lines were lost behind `| grep`; and a pipe keeps `grep` waiting while Steam, a child of the hub, still writes to it) showed the hub's order is right and the launch call returns at once, even when Steam has to start. The miss is GameMode's: inside Steam's container most processes can't load GameMode's library (`libgamemodeauto.so.0 … cannot be preloaded`, `dlopen failed - libgamemode.so`, notably for 32-bit games like Portal); when no process registers, GameMode never switches on, so there's no `start=` hook and no `--start`. Harmless for the session (it started at the pick, decision 1A); the label stays on "Starting …". Likely fix outside the hub: Fedora's 32-bit GameMode library (`gamemode.i686`), with the status item "GameMode's own tweaks never apply". The GameMode journal (`journalctl --user -u gamemoded`) shows each switch on / off by the `split_lock_mitigate` lines.

### Piece 8: rescan + Refresh

- `hub_library_rescan` (`library_store.c`): `g_list_store_remove_all`, then the Steam and platforms scans into the same store. The view and selection stay connected (the model's `items-changed`); `picked` and `running` survive because they hold their own references.
- Called from `hub_page_changed` whenever the new page is `"library"` (startup, Back, between games, after End Session). The two scan lines left `hub_build_library`: the first `add_named` makes the library visible, which rescans, and `hub->library.games` is set by then.
- `hub_build_library_page` (`library_build.c`): a vertical box with a **Refresh** button on the right (`halign` end) above the list; `hub_library_refresh` calls the rescan. The stack's `"library"` page is this box.
- Side effect: scan messages (a broken `platforms.ini` section, a disc-ID warning) repeat at every rescan.

### Piece 9: final check

Norminette clean (one 82-column line found by the review, fixed). One evening start to finish: library → Portal → plan page → Back → Sonic Heroes → Start → "Starting / Playing" → close → library with the bar → Metroid Prime at once (pad working) → close → Portal from between games → quit → End Session → quit the hub: no hub warnings or `CRITICAL`s. At rest on the library: **0.0–0.5 % CPU, ~115 MB** (milestone 6: 0.3 %, ~113 MB). The library lists **74 games** (47 Steam + 27 dumps: 5 GameCube, 6 Wii, 13 PS2, 3 PrimeHack); Jak 1–3 make 77 once they're manual entries (step 15).

**Tests (pieces 5–8):** commands arrive per platform, `(null)` for Steam; a section without `command=` skipped with its line; Sonic Heroes, Galaxy 2, Metroid Prime started from Start; a game closed → between games at once; a renamed dump → Dolphin's dialog → `ended with code 1` → between games; the hub quit while a game runs; Sonic → close → library with the bar → Galaxy 2 → Portal in the same session, countdown carrying on; a fake `Test Game (Europe) [TEST-1].iso` appearing after Back and after Refresh, gone after `rm`.

**Noted for the styling step:** GNOME won't snap the hub to half the screen (edge tiling refuses a window whose minimum size doesn't fit); likely the stack's homogeneous height plus the sections. Ideas: `gtk_stack_set_vhomogeneous(stack, FALSE)`, a scrolled window around the sections.

Bugs on the way: the set / get branches missing (the property installed but never stored), `plat.command` without `!` (every section skipped), a test print in `hub_activate` with no game picked (segfault), `plat.name` never set, an 84-column line, `g_shell_parse_argv` with the docs' parameter declaration and `&argv`, `g_new0` before `argc` was known, `g_free` on a `GError`, the fill loop over the empty `argv` / slot `i` instead of `i + 1` / `*words` / a double `g_strfreev`, `argv` never built in `hub_launch_emulator`, `running` not freed, a space in an indent, `error->code` on a `NULL` error and a `return` that skipped the close, the bar's three slots all given the label / returning the countdown / `gtk_box_append` on a center box, the bar visible in every state, the Refresh handler given `NULL`, the list built and thrown away, an 82-column line.

Code `gaming_hub` `572b9c6` … `3652674` (+ the 82-column fix).

## Step 9: the hub's library file (27–28 Nov, ~2.2 h)

The hub now remembers **every game it has ever seen** in `~/.local/share/gaming_hub/library.ini`. A rescan writes what it finds into that file, and the list is built from the file, so a game stays listed after an uninstall, when its drive is unplugged, or when its dump is moved away. Which of those it is (Ready / Drive not connected / Not installed) is step 10, worked out from each entry's `path=`.

### Decisions (piece 1)

| # | Question | Decided | Rejected |
| --- | --- | --- | --- |
| 1 | Format | **A. GKeyFile**, like `settings.ini` and `platforms.ini`: setting the keys a scan found leaves other keys alone (later `hidden=`, `universe=`, a hand-fixed name), GLib escapes the values, `g_key_file_save_to_file` is atomic (temp file + rename), readable by hand | B. JSON with json-glib (a new library, no merge for free), C. my own line format (parser + writer + escaping) |
| 2 | What identifies a game | **A. source + id**: `[steam:400]`, `[emulator:SLES-51950]`; a renamed or moved dump, a game moved to another Steam library or a renamed `platforms.ini` section stays one entry. No ID: `emulator:` + the name through `g_uri_escape_string` (`[` and `]` aren't allowed in group names). Manual entries: `manual:` + their own id at step 15 | B. source + path (a rename makes a second entry, brackets to escape), C. platform + id (a renamed section leaves ghosts) |
| 3 | What an entry stores | **A. the six properties** as keys named like the properties (`source`, `name`, `platform`, `id`, `path`, `command`), `source` as a **number**; `structs.h` says new sources go at the end of the enum only. An `.ini` holds text only, so an enum can be stored as its number or as a word of my own (GLib's registered enums would need the type registered by hand, like `HubGame`) | B. `source` as a word (two conversion functions), C. only what a scan can't give back (an unfound game couldn't be rebuilt) |
| 4 | When it's written | **A. after every rescan** (a few KB, 5–15 times an evening) | B. only when the text changed (`g_key_file_to_data` + a kept copy), C. only at exit (a crash loses everything) |
| 5 | Merge | **A. the scanners write into the key file kept in memory, then the store is built from it**: a new game makes a section, a known one gets its keys overwritten, an unfound one is left alone, unknown keys are never touched. Edit the file by hand only with the hub closed (the next save overwrites it) | B. scan into the store, then merge with a `GHashTable` of found keys, C. reload the file at every rescan |
| 6 | Missing / broken | **A.** missing → empty, quiet, created at the first save; broken → one line, renamed to `library.ini.broken` (`g_rename`), start empty; a section without `name=` → skipped with one line, kept in the file | B. don't save that run (a flag), C. overwrite it |
| 4b | Building a game from a section | **A.** an empty game (`g_object_new` with **no** property pairs) and the owned strings from `g_key_file_get_string` moved straight into its public fields; `finalize` frees them. Values skip `set_property` there, but the variadic risk step 2 guarded against isn't there | B. a `t_entry` struct of owned strings lent through `t_game_info` (a struct, two functions, every string copied twice) |

### The chain

```
startup: hub_build_library
└─ hub_library_load(&hub->library)          file_path built, file loaded (missing: empty; broken: .broken + empty)
every time the library is shown, and Refresh: hub_library_rescan
├─ hub_steam_scan(file)                     … hub_steam_app_add → hub_library_remember(file, &info)
├─ hub_platforms_scan(file)                 … hub_platform_add  → hub_library_remember(file, &info)
│     hub_library_remember                  group = hub_library_key(info); source + 5 × hub_library_set
├─ hub_library_fill(&hub->library)          store emptied, one hub_library_entry per section
└─ hub_library_save(&hub->library)          folder made (0700), file written atomically
```

| Piece | What | Where |
| --- | --- | --- |
| `t_library.file`, `.file_path` | the key file in memory (**owned**, freed with `g_key_file_free`: it's not a GObject, so no `g_object_unref` / `g_clear_object`; the `if` guards against `NULL`, which `g_key_file_free` refuses) and its path (owned) | `structs.h`, `inits_2.c`, `utils.c` |
| `hub_library_load` | path from `g_get_user_data_dir` + `HUB_DATA_DIR` + `HUB_LIBRARY_FILE`; load; on failure `g_error_matches(…, G_FILE_ERROR_NOENT)` tells missing (quiet) from broken (`g_strconcat` with `HUB_BROKEN_SUFFIX`, one line, `g_rename`); then the key file freed and made again, because a half-read file may hold half its sections. `hub_key_file_error` couldn't be reused: it returns `FALSE` for both | `library_file.c` |
| `hub_library_key` | `HUB_KEY_STEAM` / `_EMULATOR` / `_MANUAL` (`"steam:%s"` …) chosen with `if` / `else if`, then `g_strdup_printf` with the id; no id → the name through `g_uri_escape_string(name, NULL, FALSE)` (ASCII-only keys), freed after use. The caller owns the key | `library_entry.c` |
| `hub_library_set` | `g_key_file_set_string` (which creates the section if needed), or `g_key_file_remove_key` when the value is `NULL` (`set_string` refuses `NULL`, and a value that's gone mustn't stay in the file: `command` for Steam, `id` for a dump without one) | `library_entry.c` |
| `hub_library_remember` | one game into the key file: the key, `source` with `g_key_file_set_integer`, five `hub_library_set`, key freed. Replaces `hub_library_add` for the scanners | `library_entry.c` |
| Scanners | `GListStore *store` → `GKeyFile *file` in 9 functions (`steam.c`, `steam_app.c`, `platforms.c`); the locals named `file` in `hub_steam_scan` / `hub_steam_app` renamed `path` (the same name as the new parameter wouldn't compile); `hub_platform_read`'s `key_file` renamed `platform` (two key files side by side) | `steam.c`, `steam_app.c`, `platforms.c`, `library.h` |
| `hub_library_entry` | `g_object_new(hub_game_get_type(), NULL)` (one reference, mine), `source` with `g_key_file_get_integer`, five `g_key_file_get_string` moved into the fields; no `name=` → one line, else `g_list_store_append` (the store takes its own reference); `g_object_unref` (a skipped game's last: `finalize` frees its strings). A missing `source=` reads as `0` (Steam): only after a bad hand edit | `library_file.c` |
| `hub_library_fill` | `g_list_store_remove_all`, `g_key_file_get_groups` (owned strv), one entry per group, `g_strfreev` | `library_file.c` |
| `hub_library_save` | `g_path_get_dirname` (owned) → `g_mkdir_with_parents(dir, 0700)` (like `mkdir -p`; 0700 as XDG asks for data folders) → `g_key_file_save_to_file`; `FALSE` → one line with the path | `library_file.c` |
| `hub_library_rescan` | Steam scan, platforms scan, fill, save (the `remove_all` moved into the fill); `hub_library_add` deleted (no caller left) | `library_store.c` |
| `make norm` | `norminette sources includes` (not libft) | `Makefile` |

Order of the list: the file's section order (the order games were first seen; new games at the end). Sorting is step 13.

An entry (Portal):

```ini
[steam:400]
source=0
name=Portal
platform=Steam
id=400
path=/mnt/Steam2TB/SteamLibrary/steamapps/common/Portal
```

**Tests:** 74 sections after the first run (the list unchanged); a fake `Test Game (Europe) [TEST-1].iso` → 75, deleted → still 75 and still listed; the same ID under a new name → still 75, `[emulator:TEST-1]` with the new `name=` and `path=`; a line that isn't `key=value` appended with the hub closed → the error line, `library.ini.broken` kept, a fresh file with the 75 games on disk; both files and the fake disc removed → no line, 74; no `CRITICAL` on quit; norminette clean.

**Noted:** `hub_game_new` (and with it `t_game_info` as its input) has no caller since `hub_library_add` went: the scanners fill a `t_game_info` for `hub_library_remember` only. Keep or delete at the wrap-up (step 18). Steam's own duplicate manifests (a game in two libraries) now merge into one entry by app id.

Bugs on the way: `g_get_user_data_dir` given a path, `g_build_filename` without its `NULL`, `error` not set to `NULL`, `g_key_file_load_from_file` missing the path and flags, a space after `(`, the values written as the literal words (`name=name`), my example text printed for every bad section and a missing `\n`, an 81- and a 94-column line, the save's `if` the wrong way round (`error->message` on `NULL`), the fill missing from the rescan, a space missing after a comma.

Code `gaming_hub` `b4573eb` … `532d4ed`.

## Step 10: the three states and live drives (28 Nov, ~2.8 h)

Every game now shows one of three states, and they follow the drives live: unplug Steam2TB and its 47 games dim with **Plug in Steam2TB** within a second; plug it back in and they turn normal once the disk has spun up, without the window freezing. A game that isn't Ready can't be picked or started. 7 pieces: look by hand, decisions, the state on each game, the row, what can start, live drives, tests.

| State | Meaning | Row |
| --- | --- | --- |
| Ready | the disk holding the game is mounted and its `path=` exists (or the game has no path) | normal |
| Drive not connected | only systemd's `autofs` placeholder is mounted where the game's drive should be | dimmed, `Plug in Steam2TB` |
| Not installed | the disk is mounted, but `path=` is gone (uninstalled, dump deleted) | dimmed, `Not installed` |

### Piece 1: by hand

| Check | Found |
| --- | --- |
| `findmnt /mnt/Steam2TB`, plugged in | two entries on the same folder: `systemd-1 autofs` (the automount placeholder, there from boot, `timeout=0`: never unmounted by itself) and `/dev/sda1 ext4` on top |
| same, unplugged (after `umount`) | only the `autofs` line: "a non-`autofs` mount at the mount point" tells plugged from unplugged; the folder itself always exists |
| `timeout 15 ls /mnt/Steam2TB/SteamLibrary`, unplugged | `No such device` at once (exit 2): touching an unplugged automount fails immediately, no freeze |
| the same right after plugging in | waits a few seconds (spin-up + mount): touching a just-plugged disk from the main loop would freeze the window |
| `StateFlags` in all 53 manifests | all `4`; an uninstall deletes the manifest instead, so the flag can't say "not installed" |
| `lsblk -o NAME,LABEL,MOUNTPOINTS` | label `Steam2TB` = folder name; a label can't be read once the drive is unplugged |
| `gio mount -l` / `gio mount -o` | GIO sees Drive → Volume `Steam2TB` → Mount. Unplug: drive disconnected, volume removed, mount removed. Plug in: drive connected, volume added, **no mount** (automount: nothing mounted until touched). After a `umount`, GIO reports a Mount again: it counts the `autofs` placeholder. So GIO's signals are triggers, the kernel's mount list is the truth |

### Piece 2: decisions (28 Nov)

| # | Question | Decided | Rejected |
| --- | --- | --- | --- |
| 1 | How a state is worked out | **A. From the kernel's mount list** (`g_unix_mount_entries_get`, reads `/proc/self/mountinfo`, touches no disk): the longest mount path the game's `path=` is under; only `autofs` entries there → Drive not connected; else `path=` tested → Ready / Not installed. Corrected while deciding: `g_unix_mount_entry_for` (path → its mount) **touches the path** (`NULL` when it doesn't exist), so it can't tell unplugged from uninstalled and would block after a plug-in | B. A + the `fstab` entries (for drives without automount; none here), C. the path only (can't tell the two apart, may freeze) |
| 2 | Where the state lives | **A. A field on the game**, worked out after every fill and on drive signals, never saved. Changed at piece 6: also a **read-only property** (decision 6b) | B. computed in `bind` (a lookup per drawn row), C. saved in `library.ini` (stale after an unplug with the hub closed) |
| 3 | Drive not connected vs Not installed | the mount point (from the kernel), then the game's own path | |
| 4 | The name shown | **A. The mount point's last part** (`/mnt/Steam2TB` → `Steam2TB`): known while unplugged. `drive` keeps the whole mount path (piece 6 needs it); the row shows what follows its last `/`. A drive GNOME mounts by itself under `/run/media/…` vanishes completely when unplugged, so its games would show Not installed (option 1B's case) | B. the filesystem label, remembered, C. no name |
| 5 | What can start | **A. Only Ready**, checked at the pick (`hub_library_activate`, which also covers a pick between games) and at Start (`hub_launch`, in case the drive went away on the plan page) | B. Drive not connected can be planned, C. Not installed opens `steam://install/<id>` (a possible extra next to Hide) |
| 6 | Live updates | **A. GVolumeMonitor signals → re-check from the mount list; on `volume-added` also open each waiting drive in the background**, then re-check | B. a full rescan on `volume-added` (freezes the window during the spin-up), C. re-check only (a plug-in waits for Refresh) |
| 6b | How a row learns its game changed (found while testing piece 6) | **A. `state` becomes a read-only property; `g_object_notify` after each check; each row listens to its game's `notify::state` from `bind` to `unbind`** | B. rebuild the store on every signal (`picked` would keep an old object), C. swap the view's model (jumps to the top) |

### The chain

```
startup and every rescan
├─ hub_steam_scan, hub_platforms_scan      their folder opens mount any drive that's plugged in
├─ hub_library_fill                        new games, state Ready by default
├─ hub_library_check(library)              mount list once
│  └─ each game: hub_game_check(game, mounts) + g_object_notify(game, "state")
│     ├─ no path → Ready
│     ├─ hub_state_mount(mounts, path, &point)       longest mount point, any non-autofs there?
│     │  └─ hub_state_under(path, mount path)        length if inside, else 0
│     ├─ FALSE → Drive not connected, drive = copy of point
│     └─ g_file_test(path, EXISTS) → Ready / Not installed
└─ hub_library_save

rows: bind → hub_row_state + connect notify::state → hub_row_notify → hub_row_state; unbind → disconnect

GVolumeMonitor (hub_drives_watch, once at startup)
├─ volume-removed, mount-added, mount-removed → hub_drives_changed → hub_library_check
└─ volume-added → hub_drive_added
   ├─ hub_library_check
   └─ each game: hub_drive_wake(seen, game, library)  Drive not connected and drive not in the set yet
      └─ g_file_enumerate_children_async(drive)  → systemd mounts it in GIO's worker thread
         └─ hub_drive_woken → result freed → hub_library_check → rows turn Ready
```

| Piece | What | Where |
| --- | --- | --- |
| `t_game_state`, `state`, `drive` | the enum; the field; the mount point of a missing drive (owned, `NULL` otherwise, freed in `finalize`) | `structs.h`, `hub_game.c` |
| gio-unix | `gio-unix-2.0` added to both `pkg-config` lines, `<gio/gunixmounts.h>` in `structs.h` (with `<fcntl.h>` and `<glib/gstdio.h>`, moved from `gaming_hub.h`). `GUnixMountEntry` is a **current** mount (mtab), `GUnixMountPoint` an `fstab` line: same-looking getters, different types, and `node->data` is a `gpointer`, so the compiler doesn't catch a mix-up | `Makefile`, `structs.h` |
| `hub_state_under` | `"/"` → 1; not a prefix → 0; the character after the mount path must be `/` or the end | `library_state.c` |
| `hub_state_mount` | walks the list with the `mounts` parameter itself (a copy of the pointer: the caller keeps the head), keeps the longest match in `*point` (borrowed from the entry), `real` reset on a longer match and set by a non-`autofs` entry at the same length, so the order of the two stacked entries doesn't matter | `library_state.c` |
| `hub_game_check` | the only place that touches the disk, and only after the mount check; `hub_keep` copies or clears `drive` | `library_state.c` |
| `hub_library_check` | `g_unix_mount_entries_get(NULL)` (list and entries owned) → each game (`g_list_model_get_item` gives a **new reference**, given back each turn) → `g_list_free_full(…, (GDestroyNotify)g_unix_mount_entry_free)` | `library_state.c` |
| `state` property | `g_param_spec_int`, `G_PARAM_READABLE` only, so `set_property` is never called for it; `get_property` reads the field. `g_object_notify` only announces: the field is written directly | `hub_game_props.c` |
| Row | a third label; `hub_row_state` resets the opacity to 1 first (rows are reused), then 0.5 for anything not Ready; `Plug in %s` with `ft_strrchr(drive, '/') + 1`; `bind` connects `notify::state` with the row as data, `unbind` disconnects it with `g_signal_handlers_disconnect_by_data(game, row)` | `library_rows.c`, `library_build.c` |
| Start refused | `HUB_NOT_READY` line; the pick gives back its new reference before returning; `hub_launch` returns `FALSE`, so Start stays on the plan page | `library_pick.c`, `launch.c` |
| Volume monitor | `g_volume_monitor_get` (one reference, kept in `t_library.volumes`); in `hub_free_hub` the handlers are disconnected **before** the unref, because the monitor is shared in the process | `library_drives.c`, `utils.c`, `inits_2.c` |
| Background open | `g_file_new_for_path` (doesn't touch the disk), `g_file_enumerate_children_async` (the operation keeps its own reference, so the `GFile` is unreffed at once), `_finish` in the callback: an enumerator to unref, or an error to free (an unrelated disk: `No such device`, quiet). A `GHashTable` set (`g_str_hash`, `g_str_equal`, keys borrowed from the games) opens each drive once, not 47 times | `library_drives.c` |

**Found on the way:** `g_list_model_items_changed(store, 0, n, n)` with the **same** objects doesn't redraw anything: GTK's list view keeps the rows of items it already has and doesn't call `bind` again. That's why a row has to listen to its own game (6b). Refresh did redraw, because it builds new objects from the library file.

**Tests:** unplugged at startup → 47 Steam rows dimmed with `Plug in Steam2TB`, 27 dumps normal; only `umount` without unplugging → still Ready (the scan mounts the drive again: it is connected); hub open, safe unplug (`sudo umount /mnt/Steam2TB; and udisksctl power-off -b /dev/sda`) → the rows dim at once; plugged back in → normal after the spin-up, the window responsive meanwhile; a fake `Test Game (Europe) [TEST-1].iso` → Ready, deleted + Refresh → `Not installed`, double-click → `Test Game: not ready, not started` (then its section removed from `library.ini` by hand); Portal picked, drive unplugged on the plan page → Start refused; `make norm` clean; **0.0–0.5 % CPU, ~116 MB** at rest.

**Noted:** the first start after the disk has spun down waits a few seconds (the startup scan reads Steam2TB's manifests): a background startup scan is a v2 item. smartd's "unable to open ATA device" notifications came from unplugging (the disk wasn't there at its check); `smartctl -H` says PASSED.

Bugs on the way: `g_str_has_prefix`'s and `g_file_test`'s answers thrown away, the length of the wrong string, the root case returning `0`, `g_unix_mount_point_get_mount_path` (an `fstab` getter) on a mount entry, the entry itself given to `g_strcmp0` instead of its type, 26 lines (`node` dropped), branch 2 falling through into the disk test, `state` never set, a list freed before the loop, `t_hube`, `"volumes-removed"`, a space before a tab, `make` forgotten before a test.

Code `gaming_hub` `05db59b` … `620e7d4`.
