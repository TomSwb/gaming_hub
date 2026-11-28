/* ************************************************************************** */
/*                                                                            */
/*                                            :::::::    :::     :::  :::::   */
/*   steam.c                                  :+:   :+:  :+:+: :+:+:   :+:    */
/*                                            +#++:+#+   +#+ +:+ +#+   +#+    */
/*   By: TomSwb <github.com/TomSwb>           +#+   +#+  +#+     +#+   +#+    */
/*                                            ########   ###     ###  #####   */
/*   Created: 2026/11/23 20:31:20 by TomSwb                                   */
/*   Updated: 2026/11/23 20:31:20 by TomSwb                   bitminded.ch    */
/*                                                                            */
/* ************************************************************************** */

#include "gaming_hub.h"

void	hub_steam_scan(GKeyFile *file)
{
	gchar		*path;
	gchar		*buffer;
	t_kv_cursor	cursor;

	path = g_build_filename(g_get_home_dir(), HUB_STEAM_FOLDERS, NULL);
	buffer = hub_kv_load(path, &cursor);
	if (!buffer)
	{
		g_free(path);
		return ;
	}
	if (!hub_kv_enter(&cursor, HUB_STEAM_ROOT)
		|| !hub_steam_folders(file, &cursor))
	{
		g_printerr("%s: line %d: not a valid Steam library list\n", path,
			cursor.line);
	}
	g_free(buffer);
	g_free(path);
}

gboolean	hub_steam_folders(GKeyFile *file, t_kv_cursor *cursor)
{
	t_kv_token	token;
	gchar		*name;
	gchar		*value;
	gchar		*path;

	token = hub_kv_pair(cursor, &name, &value);
	while (token == KV_STRING || token == KV_OPEN)
	{
		g_free(name);
		g_free(value);
		path = NULL;
		if (token == KV_OPEN)
			path = hub_steam_lib_path(cursor);
		if (token == KV_OPEN && path == NULL)
			token = KV_ERROR;
		else
			token = hub_kv_pair(cursor, &name, &value);
		if (path)
		{
			hub_steam_library(file, path);
			g_free(path);
		}
	}
	return (token == KV_CLOSE);
}

void	hub_steam_library(GKeyFile *file, const gchar *lib)
{
	gchar		*path;
	GDir		*dir;
	const gchar	*name;

	path = g_build_filename(lib, HUB_STEAM_APPS, NULL);
	dir = g_dir_open(path, 0, NULL);
	if (!dir)
	{
		g_free(path);
		return ;
	}
	name = g_dir_read_name(dir);
	while (name)
	{
		if (g_str_has_prefix(name, HUB_STEAM_PREFIX)
			&& g_str_has_suffix(name, HUB_STEAM_SUFFIX))
			hub_steam_app(file, path, name);
		name = g_dir_read_name(dir);
	}
	g_dir_close(dir);
	g_free(path);
}

gchar	*hub_steam_lib_path(t_kv_cursor *cursor)
{
	t_kv_token	token;
	gchar		*name;
	gchar		*value;
	gchar		*path;

	path = NULL;
	token = hub_kv_pair(cursor, &name, &value);
	while (token == KV_STRING || token == KV_OPEN)
	{
		if (token == KV_STRING && g_strcmp0(name, HUB_STEAM_PATH) == 0)
			hub_kv_take(&path, &value);
		g_free(name);
		g_free(value);
		if (token == KV_OPEN && !hub_kv_skip_block(cursor))
			token = KV_ERROR;
		else
			token = hub_kv_pair(cursor, &name, &value);
	}
	if (token == KV_ERROR)
	{
		g_free(path);
		return (NULL);
	}
	return (path);
}

void	hub_steam_app_free(t_steam_app *app)
{
	g_free(app->name);
	g_free(app->id);
	g_free(app->dir);
	g_free(app->owner);
}
