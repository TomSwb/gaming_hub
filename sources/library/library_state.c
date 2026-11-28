/* ************************************************************************** */
/*                                                                            */
/*                                            :::::::    :::     :::  :::::   */
/*   library_state.c                          :+:   :+:  :+:+: :+:+:   :+:    */
/*                                            +#++:+#+   +#+ +:+ +#+   +#+    */
/*   By: TomSwb <github.com/TomSwb>           +#+   +#+  +#+     +#+   +#+    */
/*                                            ########   ###     ###  #####   */
/*   Created: 2026/11/28 12:27:25 by TomSwb                                   */
/*   Updated: 2026/11/28 12:27:25 by TomSwb                   bitminded.ch    */
/*                                                                            */
/* ************************************************************************** */

#include "gaming_hub.h"

gsize	hub_state_under(const gchar *path, const gchar *point)
{
	gsize	len;

	if (g_strcmp0(point, HUB_ROOT_PATH) == 0)
		return (1);
	if (!g_str_has_prefix(path, point))
		return (0);
	len = ft_strlen(point);
	if (path[len] == HUB_ROOT_DIR || path[len] == '\0')
		return (len);
	return (0);
}

gboolean	hub_state_mount(GList *mounts, const gchar *path,
				const gchar **point)
{
	const gchar	*here;
	gsize		len;
	gsize		best;
	gboolean	real;

	best = 0;
	real = FALSE;
	*point = NULL;
	while (mounts)
	{
		here = g_unix_mount_entry_get_mount_path(mounts->data);
		len = hub_state_under(path, here);
		if (len > best)
		{
			best = len;
			*point = here;
			real = FALSE;
		}
		if (len == best && len != 0
			&& g_strcmp0(g_unix_mount_entry_get_fs_type(mounts->data),
				HUB_AUTOFS) != 0)
			real = TRUE;
		mounts = mounts->next;
	}
	return (real);
}

void	hub_game_check(t_hub_game *game, GList *mounts)
{
	const gchar	*point;

	if (game->path == NULL)
	{
		game->state = GAME_READY;
		hub_keep(&game->drive, NULL);
		return ;
	}
	if (!hub_state_mount(mounts, game->path, &point))
	{
		game->state = GAME_DRIVE_NOT_CONNECTED;
		hub_keep(&game->drive, point);
		return ;
	}
	hub_keep(&game->drive, NULL);
	if (g_file_test(game->path, G_FILE_TEST_EXISTS))
		game->state = GAME_READY;
	else
		game->state = GAME_NOT_INSTALLED;
}

void	hub_library_check(t_library *library)
{
	GList		*mounts;
	guint		n;
	guint		i;
	t_hub_game	*game;

	mounts = g_unix_mount_entries_get(NULL);
	n = g_list_model_get_n_items(G_LIST_MODEL(library->games));
	i = 0;
	while (i < n)
	{
		game = hub_game_cast(
				g_list_model_get_item(G_LIST_MODEL(library->games), i));
		hub_game_check(game, mounts);
		g_object_notify(G_OBJECT(game), HUB_GAME_STATE);
		g_object_unref(game);
		i++;
	}
	g_list_free_full(mounts, (GDestroyNotify)g_unix_mount_entry_free);
}

gboolean	hub_state_present(const gchar *path)
{
	GList		*mounts;
	const gchar	*point;
	gboolean	present;

	mounts = g_unix_mount_entries_get(NULL);
	present = hub_state_mount(mounts, path, &point);
	g_list_free_full(mounts, (GDestroyNotify)g_unix_mount_entry_free);
	return (present);
}
