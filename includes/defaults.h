/* ************************************************************************** */
/*                                                                            */
/*                                            :::::::    :::     :::  :::::   */
/*   defaults.h                               :+:   :+:  :+:+: :+:+:   :+:    */
/*                                            +#++:+#+   +#+ +:+ +#+   +#+    */
/*   By: TomSwb <github.com/TomSwb>           +#+   +#+  +#+     +#+   +#+    */
/*                                            ########   ###     ###  #####   */
/*   Created: 2026/11/11 08:45:19 by TomSwb                                   */
/*   Updated: 2026/11/23 22:01:33 by TomSwb                   bitminded.ch    */
/*                                                                            */
/* ************************************************************************** */

#ifndef DEFAULTS_H
# define DEFAULTS_H

// === Settings === //

# define HUB_DEF_SESSION 120
# define HUB_SESSION_LOWEST 5
# define HUB_SESSION_HIGHEST 360
# define HUB_SESSION_STEP 5

# define HUB_BAT_LOW 25

# define HUB_DEF_WIND_DOWN 10
# define HUB_WIND_DOWN_LOWEST 1
# define HUB_WIND_DOWN_HIGHEST 30
# define HUB_WIND_DOWN_STEP 1

# define HUB_DEF_BREAK_EVERY 50
# define HUB_BREAK_MIN_EVERY 5 // MIN = minimum
# define HUB_BREAK_MAX_EVERY 90
# define HUB_BREAK_STEP 5

# define HUB_DEF_EYE_CUE_MIN 20
# define HUB_DEF_EYE_CUE_LENGTH 20

# define HUB_SOUND "/usr/share/sounds/freedesktop/stereo/bell.oga"
# define HUB_DEF_POPUPS TRUE
# define HUB_DEF_SOUND TRUE

# define HUB_BLUEZ_NAME "org.bluez"
# define HUB_BLUEZ_DEVICE "org.bluez.Device1"
# define HUB_BLUEZ_BATTERY "org.bluez.Battery1"
# define HUB_PA_BT_ADDRESS "api.bluez5.address"
# define HUB_PA_SHORT_NAME "device.profile.description"
# define HUB_PA_KEY "sink"
# define HUB_PA_ADDR "address"

# define HUB_PPD_NAME "org.freedesktop.UPower.PowerProfiles"
# define HUB_PPD_PATH "/org/freedesktop/UPower/PowerProfiles"
# define HUB_PPD_IFACE "org.freedesktop.UPower.PowerProfiles"
# define HUB_PPD_ACTIVE "ActiveProfile"
# define HUB_PPD_PROFILES "Profiles"
# define HUB_PPD_KEY "profile"
# define HUB_PPD_METH_NAME "org.freedesktop.DBus.Properties.Set"

# define HUB_GAME_TYPE_NAME "HubGame"
# define HUB_GAME_NAME "name"
# define HUB_GAME_SOURCE "source"
# define HUB_GAME_PLATFORM "platform"
# define HUB_GAME_ID "id"
# define HUB_GAME_PATH "path"

# define HUB_STEAM_PATH "path"
# define HUB_STEAM_ROOT "libraryfolders"
# define HUB_STEAM_FOLDERS ".local/share/Steam/steamapps/libraryfolders.vdf"
# define HUB_STEAM_APPS "steamapps"
# define HUB_STEAM_PREFIX "appmanifest_"
# define HUB_STEAM_SUFFIX ".acf"
# define HUB_STEAM_ID "appid"
# define HUB_STEAM_NAME "name"
# define HUB_STEAM_DIR "installdir"
# define HUB_STEAM_OWNER "LastOwner"
# define HUB_STEAM_SKIP "228980;4099820"
# define HUB_STEAM_NO_OWNER "0"
# define HUB_STEAM_SEP ";"
# define HUB_STEAM_COMMON "common"
# define HUB_STEAM_PLATFORM "Steam"
# define HUB_STEAM_STATE "AppState"
# define HUB_STEAM_RUN "steam://rungameid/%s"

# define HUB_CONFIG_DIR "gaming_hub"
# define HUB_PLATFORMS_FILE "platforms.ini"
# define HUB_PLAT_FOLDER "folder"
# define HUB_PLAT_EXTS "extensions"
# define HUB_HOME_PREFIX "~/"
# define HUB_HOME_PREFIX_L 2

# define HUB_DISC_HEADER 32
# define HUB_DISC_ID_LEN 6
# define HUB_WII_OFFSET 24
# define HUB_WII_MAGIC "\x5D\x1C\x9E\xA3"
# define HUB_GC_OFFSET 28
# define HUB_GC_MAGIC "\xC2\x33\x9F\x3D"
# define HUB_MAGIC_LEN 4

# define HUB_STARTING "Starting %s ..."
# define HUB_PLAYING "Playing %s"

#endif
