/* ************************************************************************** */
/*                                                                            */
/*                                            :::::::    :::     :::  :::::   */
/*   steam_app.c                              :+:   :+:  :+:+: :+:+:   :+:    */
/*                                            +#++:+#+   +#+ +:+ +#+   +#+    */
/*   By: TomSwb <github.com/TomSwb>           +#+   +#+  +#+     +#+   +#+    */
/*                                            ########   ###     ###  #####   */
/*   Created: 2026/11/23 20:31:29 by TomSwb                                   */
/*   Updated: 2026/11/23 20:31:29 by TomSwb                   bitminded.ch    */
/*                                                                            */
/* ************************************************************************** */

#include "gaming_hub.h"

void	hub_steam_app(GListStore *store, const gchar *folder,
			const gchar *name)
{
	gchar		*file;
	gchar		*buffer;
	t_kv_cursor	cursor;
	t_steam_app	app;

	file = g_build_filename(folder, name, NULL);
	buffer = hub_kv_load(file, &cursor);
	if (!buffer)
	{
		g_free(file);
		return ;
	}
	ft_bzero(&app, sizeof(app));
	if (!hub_kv_enter(&cursor, HUB_STEAM_STATE)
		|| !hub_steam_app_read(&cursor, &app))
		g_printerr("%s, line %d, not a valid Steam manifest\n",
			file, cursor.line);
	else if (!hub_steam_app_skip(&app))
		hub_steam_app_add(store, &app, folder);
	hub_steam_app_free(&app);
	g_free(buffer);
	g_free(file);
}

void	hub_steam_app_keep(t_steam_app *app, const gchar *key,
			gchar **value)
{
	if (g_strcmp0(key, HUB_STEAM_ID) == 0)
		hub_kv_take(&app->id, value);
	else if (g_strcmp0(key, HUB_STEAM_NAME) == 0)
		hub_kv_take(&app->name, value);
	else if (g_strcmp0(key, HUB_STEAM_DIR) == 0)
		hub_kv_take(&app->dir, value);
	else if (g_strcmp0(key, HUB_STEAM_OWNER) == 0)
		hub_kv_take(&app->owner, value);
}

gboolean	hub_steam_app_read(t_kv_cursor *cursor, t_steam_app *app)
{
	t_kv_token	token;
	gchar		*name;
	gchar		*value;

	token = hub_kv_pair(cursor, &name, &value);
	while (token == KV_STRING || token == KV_OPEN)
	{
		if (token == KV_STRING)
			hub_steam_app_keep(app, name, &value);
		g_free(name);
		g_free(value);
		if (token == KV_OPEN && !hub_kv_skip_block(cursor))
			token = KV_ERROR;
		else
			token = hub_kv_pair(cursor, &name, &value);
	}
	return (token == KV_CLOSE);
}

gboolean	hub_steam_app_skip(const t_steam_app *app)
{
	gchar		**list;
	gboolean	skip;

	if (!app->id || !app->name || !app->dir)
		return (TRUE);
	if (g_strcmp0(app->owner, HUB_STEAM_NO_OWNER) == 0)
		return (TRUE);
	list = g_strsplit(HUB_STEAM_SKIP, HUB_STEAM_SEP, -1);
	skip = g_strv_contains((const gchar * const *)list, app->id);
	g_strfreev(list);
	return (skip);
}

void	hub_steam_app_add(GListStore *store, t_steam_app *app,
			const gchar *folder)
{
	t_game_info	info;
	gchar		*path;

	g_strstrip(app->name);
	path = g_build_filename(folder, HUB_STEAM_COMMON, app->dir, NULL);
	info.source = GAME_STEAM;
	info.platform = HUB_STEAM_PLATFORM;
	info.name = app->name;
	info.id = app->id;
	info.command = NULL;
	info.path = path;
	hub_library_add(store, &info);
	g_free(path);
}
