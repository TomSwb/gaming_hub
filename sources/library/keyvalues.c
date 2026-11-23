/* ************************************************************************** */
/*                                                                            */
/*                                            :::::::    :::     :::  :::::   */
/*   keyvalues.c                              :+:   :+:  :+:+: :+:+:   :+:    */
/*                                            +#++:+#+   +#+ +:+ +#+   +#+    */
/*   By: TomSwb <github.com/TomSwb>           +#+   +#+  +#+     +#+   +#+    */
/*                                            ########   ###     ###  #####   */
/*   Created: 2026/11/23 18:08:19 by TomSwb                                   */
/*   Updated: 2026/11/23 18:08:19 by TomSwb                   bitminded.ch    */
/*                                                                            */
/* ************************************************************************** */

#include "gaming_hub.h"

gboolean	hub_kv_enter(t_kv_cursor *cursor, const gchar *key)
{
	t_kv_token	token;
	gchar		*name;
	gchar		*value;
	gboolean	found;

	while (TRUE)
	{
		token = hub_kv_next(cursor, &name);
		if (token != KV_STRING)
			return (FALSE);
		token = hub_kv_next(cursor, &value);
		found = g_strcmp0(name, key) == 0;
		g_free(name);
		g_free(value);
		if (token == KV_OPEN && found)
			return (TRUE);
		else if (token == KV_OPEN)
		{
			if (!hub_kv_skip_block(cursor))
				return (FALSE);
		}
		else if (token != KV_STRING)
			return (FALSE);
	}
}

t_kv_token	hub_kv_next(t_kv_cursor *cursor, gchar **text)
{
	*text = NULL;
	hub_kv_skip_space(cursor);
	if (*cursor->pos == '\0')
		return (KV_END);
	else if (*cursor->pos == '{')
	{
		cursor->pos++;
		return (KV_OPEN);
	}
	else if (*cursor->pos == '}')
	{
		cursor->pos++;
		return (KV_CLOSE);
	}
	else if (*cursor->pos == '"')
		return (hub_kv_read_string(cursor, text));
	return (KV_ERROR);
}

void	hub_kv_skip_space(t_kv_cursor *cursor)
{
	while (g_ascii_isspace(*cursor->pos))
	{
		if (*cursor->pos == '\n')
			cursor->line++;
		cursor->pos++;
	}
}

t_kv_token	hub_kv_read_string(t_kv_cursor *cursor, gchar **text)
{
	const gchar	*start;

	if (*cursor->pos != '"')
		return (KV_ERROR);
	cursor->pos++;
	start = cursor->pos;
	while (*cursor->pos && *cursor->pos != '"')
	{
		if (*cursor->pos == '\\' && *(cursor->pos + 1) != '\0')
			cursor->pos++;
		if (*cursor->pos == '\n')
			cursor->line++;
		cursor->pos++;
	}
	if (*cursor->pos == '\0')
		return (KV_ERROR);
	*text = g_strndup(start, cursor->pos - start);
	cursor->pos++;
	return (KV_STRING);
}

gboolean	hub_kv_skip_block(t_kv_cursor *cursor)
{
	t_kv_token	token;
	gint		depth;
	gchar		*text;

	depth = 1;
	while (depth > 0)
	{
		token = hub_kv_next(cursor, &text);
		if (token == KV_OPEN)
			depth++;
		else if (token == KV_CLOSE)
			depth--;
		else if (token == KV_END || token == KV_ERROR)
			return (FALSE);
		g_free(text);
	}
	return (TRUE);
}
