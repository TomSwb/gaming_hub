/* ************************************************************************** */
/*                                                                            */
/*                                            :::::::    :::     :::  :::::   */
/*   library_file.c                           :+:   :+:  :+:+: :+:+:   :+:    */
/*                                            +#++:+#+   +#+ +:+ +#+   +#+    */
/*   By: TomSwb <github.com/TomSwb>           +#+   +#+  +#+     +#+   +#+    */
/*                                            ########   ###     ###  #####   */
/*   Created: 2026/11/28 10:06:57 by TomSwb                                   */
/*   Updated: 2026/11/28 10:06:57 by TomSwb                   bitminded.ch    */
/*                                                                            */
/* ************************************************************************** */

#include "gaming_hub.h"

void	hub_library_load(t_library *library)
{
	GError	*error;
	gchar	*broken;

	error = NULL;
	library->file_path = g_build_filename(g_get_user_data_dir(),
			HUB_DATA_DIR, HUB_LIBRARY_FILE, NULL);
	library->file = g_key_file_new();
	if (g_key_file_load_from_file(library->file, library->file_path,
			G_KEY_FILE_NONE, &error))
		return ;
	if (!g_error_matches(error, G_FILE_ERROR, G_FILE_ERROR_NOENT))
	{
		broken = g_strconcat(library->file_path, HUB_BROKEN_SUFFIX, NULL);
		g_printerr("%s: %s - kept as %s, starting fresh\n",
			library->file_path, error->message, broken);
		g_rename(library->file_path, broken);
		g_free(broken);
	}
	g_error_free(error);
	g_key_file_free(library->file);
	library->file = g_key_file_new();
}

void	hub_library_entry(GListStore *store, GKeyFile *file,
			const gchar *group)
{
	t_hub_game	*game;

	game = g_object_new(hub_game_get_type(), NULL);
	game->source = g_key_file_get_integer(file, group, HUB_GAME_SOURCE, NULL);
	game->name = g_key_file_get_string(file, group, HUB_GAME_NAME, NULL);
	game->platform = g_key_file_get_string(file, group,
			HUB_GAME_PLATFORM, NULL);
	game->id = g_key_file_get_string(file, group, HUB_GAME_ID, NULL);
	game->path = g_key_file_get_string(file, group, HUB_GAME_PATH, NULL);
	game->command = g_key_file_get_string(file, group, HUB_GAME_COMMAND, NULL);
	if (game->name == NULL)
		g_printerr("%s: [%s] has no %s=, skipped\n",
			HUB_LIBRARY_FILE, group, HUB_GAME_NAME);
	else
		g_list_store_append(store, game);
	g_object_unref(game);
}

void	hub_library_fill(t_library *library)
{
	gchar	**groups;
	gsize	i;

	g_list_store_remove_all(library->games);
	groups = g_key_file_get_groups(library->file, NULL);
	i = 0;
	while (groups[i])
	{
		hub_library_entry(library->games, library->file, groups[i]);
		i++;
	}
	g_strfreev(groups);
}

void	hub_library_save(t_library *library)
{
	gchar	*dir;
	GError	*error;

	dir = g_path_get_dirname(library->file_path);
	g_mkdir_with_parents(dir, 0700);
	g_free(dir);
	error = NULL;
	if (!g_key_file_save_to_file(library->file, library->file_path, &error))
	{
		g_printerr("%s: %s\n", library->file_path, error->message);
		g_error_free(error);
	}
}
