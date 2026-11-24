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
			hub_platform_read(store, key_file, groups[i]);
			i++;
		}
		g_strfreev(groups);
	}
	g_free(path);
	g_key_file_free(key_file);
}

void	hub_platform_read(GListStore *store, GKeyFile *key_file,
			const gchar *group)
{
	gchar	*folder;
	gchar	**extensions;
	guint	i;
	guint	j;

	(void)store;
	j = 0;
	folder = g_key_file_get_string(key_file, group, HUB_PLAT_FOLDER, NULL);
	extensions = g_key_file_get_string_list(key_file, group,
			HUB_PLAT_EXTS, NULL, NULL);
	if (!folder || !extensions)
		g_printerr("%s: [%s] needs %s= and %s=\n", HUB_PLATFORMS_FILE, group, HUB_PLAT_FOLDER, HUB_PLAT_EXTS);
	else
	{
		i = g_strv_length(extensions);
		g_print("group: %s, file: %s, #extensions: %u\n", group, folder, i);
		while (j < i)
		{
			g_print("extensions: %s\n", extensions[j]);
			j++;
		}
	}
	g_free(folder);
	g_strfreev(extensions);
}
