/* ************************************************************************** */
/*                                                                            */
/*                                            :::::::    :::     :::  :::::   */
/*   platforms.c                              :+:   :+:  :+:+: :+:+:   :+:    */
/*                                            +#++:+#+   +#+ +:+ +#+   +#+    */
/*   By: TomSwb <github.com/TomSwb>           +#+   +#+  +#+     +#+   +#+    */
/*                                            ########   ###     ###  #####   */
/*   Created: 2026/11/24 12:02:22 by TomSwb                                   */
/*   Updated: 2026/11/24 12:02:22 by TomSwb                   bitminded.ch    */
/*                                                                            */
/* ************************************************************************** */

#include "gaming_hub.h"

void	hub_platforms_scan(GListStore *store)
{
	gchar		*path;
	GKeyFile	*key_file;
	gchar		**groups;
	gsize		i;

	(void)store;
	i = 0;
	path = g_build_filename(g_get_user_config_dir(),
			HUB_CONFIG_DIR, HUB_PLATFORMS_FILE, NULL);
	key_file = g_key_file_new();
	if (hub_key_file_error(key_file, path))
	{
		groups = g_key_file_get_groups(key_file, NULL);
		while (groups[i])
		{
			g_print("%s\n", groups[i]);
			i++;
		}
		g_strfreev(groups);
	}
	g_free(path);
	g_key_file_free(key_file);
}
