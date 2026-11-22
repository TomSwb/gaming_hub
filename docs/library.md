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
