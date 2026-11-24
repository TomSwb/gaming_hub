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
| 7 | Names from file names, disc header check | 1–2 h |
| 8 | Launch and the session loop (`steam://`, emulators watched as a process); rescan when the library page is shown + a Refresh button (added 23 Nov; live folder watching is v2) | 4.5–7 h |
| 9 | The hub's library file | 2–3 h |
| 10 | Three states, live drives | 2–3 h |
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
