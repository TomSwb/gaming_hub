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
| 8 | Launch and the session loop (`steam://`, emulators watched as a process) | 4–6 h |
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

