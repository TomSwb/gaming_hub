/* ************************************************************************** */
/*                                                                            */
/*                                            :::::::    :::     :::  :::::   */
/*   library_drives.c                         :+:   :+:  :+:+: :+:+:   :+:    */
/*                                            +#++:+#+   +#+ +:+ +#+   +#+    */
/*   By: TomSwb <github.com/TomSwb>           +#+   +#+  +#+     +#+   +#+    */
/*                                            ########   ###     ###  #####   */
/*   Created: 2026/11/28 13:37:22 by TomSwb                                   */
/*   Updated: 2026/11/28 13:37:22 by TomSwb                   bitminded.ch    */
/*                                                                            */
/* ************************************************************************** */

#include "gaming_hub.h"

void	hub_drives_changed(GVolumeMonitor *self, gpointer object,
			gpointer user_data)
{
	t_hub	*hub;

	(void)self;
	(void)object;
	hub = user_data;
	hub_library_check(&hub->library);
}

void	hub_drives_watch(t_hub *hub)
{
	GVolumeMonitor	*volumes;

	volumes = g_volume_monitor_get();
	hub->library.volumes = volumes;
	g_signal_connect(volumes, "volume-removed",
		G_CALLBACK(hub_drives_changed), hub);
	g_signal_connect(volumes, "mount-added",
		G_CALLBACK(hub_drives_changed), hub);
	g_signal_connect(volumes, "mount-removed",
		G_CALLBACK(hub_drives_changed), hub);
}
