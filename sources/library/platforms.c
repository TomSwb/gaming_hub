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

	folder = g_key_file_get_string(key_file, group, HUB_PLAT_FOLDER, NULL);
	extensions = g_key_file_get_string_list(key_file, group,
			HUB_PLAT_EXTS, NULL, NULL);
	if (!folder || !extensions)
		g_printerr("%s: [%s] needs %s= and %s=\n", HUB_PLATFORMS_FILE,
			group, HUB_PLAT_FOLDER, HUB_PLAT_EXTS);
	else
	{
		hub_path_expand(&folder);
		hub_platform_folder(store, group, folder, extensions);
	}
	g_free(folder);
	g_strfreev(extensions);
}

gboolean	hub_platform_ext_ok(const gchar *name, gchar **extensions)
{
	const gchar	*dot;
	gsize		i;

	dot = ft_strrchr(name, '.');
	if (!dot || dot == name)
		return (FALSE);
	dot++;
	i = 0;
	while (extensions[i])
	{
		if (g_ascii_strcasecmp(dot, extensions[i]) == 0)
			return (TRUE);
		i++;
	}
	return (FALSE);
}

void	hub_platform_folder(GListStore *store, const gchar *group,
			const gchar *folder, gchar **extensions)
{
	GDir		*dir;
	const gchar	*name;

	dir = g_dir_open(folder, 0, NULL);
	if (!dir)
		return ;
	name = g_dir_read_name(dir);
	while (name)
	{
		if (hub_platform_ext_ok(name, extensions))
			hub_platform_add(store, group, folder, name);
		name = g_dir_read_name(dir);
	}
	g_dir_close(dir);
}

void	hub_platform_add(GListStore *store, const gchar *group,
			const gchar *folder, const gchar *name)
{
	t_game_info	info;
	gchar		*path;
	t_dump_name	dump;

	hub_name_parse(name, &dump);
	path = g_build_filename(folder, name, NULL);
	hub_disc_check(path, &dump);
	info.source = GAME_EMULATOR;
	info.platform = group;
	info.name = dump.title;
	info.id = dump.id;
	info.path = path;
	hub_library_add(store, &info);
	hub_name_clear(&dump);
	g_free(path);
}
