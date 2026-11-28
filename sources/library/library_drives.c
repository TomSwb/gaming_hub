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
	g_signal_connect(volumes, "volume-added",
		G_CALLBACK(hub_drive_added), hub);
}

void	hub_drive_woken(GObject *source, GAsyncResult *result,
			gpointer user_data)
{
	GFileEnumerator	*files;
	GError			*error;

	error = NULL;
	files = g_file_enumerate_children_finish(G_FILE(source), result,
			&error);
	if (files)
		g_object_unref(files);
	else
		g_error_free(error);
	hub_library_check(user_data);
}

void	hub_drive_wake(GHashTable *seen, t_hub_game *game,
			t_library *library)
{
	GFile	*file;

	if (game->state != GAME_DRIVE_NOT_CONNECTED
		|| !g_hash_table_add(seen, game->drive))
		return ;
	file = g_file_new_for_path(game->drive);
	g_file_enumerate_children_async(file, G_FILE_ATTRIBUTE_STANDARD_NAME,
		G_FILE_QUERY_INFO_NONE, G_PRIORITY_DEFAULT, NULL,
		hub_drive_woken, library);
	g_object_unref(file);
}

void	hub_drive_added(GVolumeMonitor *self, GVolume *volume,
			gpointer user_data)
{
	t_hub		*hub;
	GHashTable	*seen;
	guint		n;
	guint		i;
	t_hub_game	*game;

	(void)self;
	(void)volume;
	hub = user_data;
	hub_library_check(&hub->library);
	seen = g_hash_table_new(g_str_hash, g_str_equal);
	n = g_list_model_get_n_items(G_LIST_MODEL(hub->library.games));
	i = 0;
	while (i < n)
	{
		game = hub_game_cast(
				g_list_model_get_item(G_LIST_MODEL(hub->library.games), i));
		hub_drive_wake(seen, game, &hub->library);
		g_object_unref(game);
		i++;
	}
	g_hash_table_destroy(seen);
}
