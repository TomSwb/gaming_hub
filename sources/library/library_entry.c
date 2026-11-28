/* ************************************************************************** */
/*                                                                            */
/*                                            :::::::    :::     :::  :::::   */
/*   library_entry.c                          :+:   :+:  :+:+: :+:+:   :+:    */
/*                                            +#++:+#+   +#+ +:+ +#+   +#+    */
/*   By: TomSwb <github.com/TomSwb>           +#+   +#+  +#+     +#+   +#+    */
/*                                            ########   ###     ###  #####   */
/*   Created: 2026/11/28 10:48:31 by TomSwb                                   */
/*   Updated: 2026/11/28 10:48:31 by TomSwb                   bitminded.ch    */
/*                                                                            */
/* ************************************************************************** */

#include "gaming_hub.h"

gchar	*hub_library_key(const t_game_info *info)
{
	const gchar	*format;
	gchar		*escaped;
	gchar		*key;

	if (info->source == GAME_STEAM)
		format = HUB_KEY_STEAM;
	else if (info->source == GAME_EMULATOR)
		format = HUB_KEY_EMULATOR;
	else
		format = HUB_KEY_MANUAL;
	if (info->id)
		return (g_strdup_printf(format, info->id));
	else
	{
		escaped = g_uri_escape_string(info->name, NULL, FALSE);
		key = g_strdup_printf(format, escaped);
		g_free(escaped);
		return (key);
	}
}

void	hub_library_set(GKeyFile *file, const gchar *group,
			const gchar *key, const gchar *value)
{
	if (value)
		g_key_file_set_string(file, group, key, value);
	else
	 	g_key_file_remove_key(file, group, key, NULL);
}

void	hub_library_remember(GKeyFile *file, const t_game_info *info)
{
	gchar	*group;

	group = hub_library_key(info);
	g_key_file_set_integer(file, group, HUB_GAME_SOURCE, info->source);
	hub_library_set(file, group, HUB_GAME_NAME, info->name);
	hub_library_set(file, group, HUB_GAME_PLATFORM, info->platform);
	hub_library_set(file, group, HUB_GAME_ID, info->id);
	hub_library_set(file, group, HUB_GAME_PATH, info->path);
	hub_library_set(file, group, HUB_GAME_COMMAND, info->command);
	g_free(group);
}
