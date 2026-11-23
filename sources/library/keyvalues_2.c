/* ************************************************************************** */
/*                                                                            */
/*                                            :::::::    :::     :::  :::::   */
/*   keyvalues_2.c                            :+:   :+:  :+:+: :+:+:   :+:    */
/*                                            +#++:+#+   +#+ +:+ +#+   +#+    */
/*   By: TomSwb <github.com/TomSwb>           +#+   +#+  +#+     +#+   +#+    */
/*                                            ########   ###     ###  #####   */
/*   Created: 2026/11/23 20:31:05 by TomSwb                                   */
/*   Updated: 2026/11/23 20:31:05 by TomSwb                   bitminded.ch    */
/*                                                                            */
/* ************************************************************************** */

#include "gaming_hub.h"

gchar	*hub_kv_load(const gchar *path, t_kv_cursor *cursor)
{
	gchar	*contents;
	GError	*error;

	error = NULL;
	if (!g_file_get_contents(path, &contents, NULL, &error))
	{
		g_printerr("%s\n", error->message);
		g_error_free(error);
		return (NULL);
	}
	cursor->pos = contents;
	cursor->line = 1;
	return (contents);
}

t_kv_token	hub_kv_pair(t_kv_cursor *cursor, gchar **name,
				gchar **value)
{
	t_kv_token	token;

	token = hub_kv_next(cursor, name);
	if (token != KV_STRING)
	{
		if (token == KV_CLOSE)
			return (KV_CLOSE);
		else
			return (KV_ERROR);
	}
	token = hub_kv_next(cursor, value);
	if (token == KV_STRING || token == KV_OPEN)
		return (token);
	g_free(*name);
	*name = NULL;
	return (KV_ERROR);
}

void	hub_kv_take(gchar **slot, gchar **value)
{
	g_free(*slot);
	*slot = *value;
	*value = NULL;
}
