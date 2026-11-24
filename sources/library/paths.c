/* ************************************************************************** */
/*                                                                            */
/*                                            :::::::    :::     :::  :::::   */
/*   paths.c                                  :+:   :+:  :+:+: :+:+:   :+:    */
/*                                            +#++:+#+   +#+ +:+ +#+   +#+    */
/*   By: TomSwb <github.com/TomSwb>           +#+   +#+  +#+     +#+   +#+    */
/*                                            ########   ###     ###  #####   */
/*   Created: 2026/11/24 13:12:20 by TomSwb                                   */
/*   Updated: 2026/11/24 13:12:20 by TomSwb                   bitminded.ch    */
/*                                                                            */
/* ************************************************************************** */

#include "gaming_hub.h"

void	hub_path_expand(gchar **path)
{
	gchar	*full;

	if (!path || !*path || !g_str_has_prefix(*path, HUB_HOME_PREFIX))
		return ;
	full = g_build_filename(g_get_home_dir(), *path + HUB_HOME_PREFIX_L, NULL);
	g_free(*path);
	*path = full;
}
