/* ************************************************************************** */
/*                                                                            */
/*                                            :::::::    :::     :::  :::::   */
/*   defaults.h                               :+:   :+:  :+:+: :+:+:   :+:    */
/*                                            +#++:+#+   +#+ +:+ +#+   +#+    */
/*   By: TomSwb <github.com/TomSwb>           +#+   +#+  +#+     +#+   +#+    */
/*                                            ########   ###     ###  #####   */
/*   Created: 2026/11/11 08:45:19 by TomSwb                                   */
/*   Updated: 2026/11/11 08:45:19 by TomSwb                   bitminded.ch    */
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

#endif
