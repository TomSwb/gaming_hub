/* ************************************************************************** */
/*                                                                            */
/*                                            :::::::    :::     :::  :::::   */
/*   settings.c                               :+:   :+:  :+:+: :+:+:   :+:    */
/*                                            +#++:+#+   +#+ +:+ +#+   +#+    */
/*   By: TomSwb <github.com/TomSwb>           +#+   +#+  +#+     +#+   +#+    */
/*                                            ########   ###     ###  #####   */
/*   Created: 2026/11/11 09:17:09 by TomSwb                                   */
/*   Updated: 2026/11/11 09:17:09 by TomSwb                   bitminded.ch    */
/*                                                                            */
/* ************************************************************************** */

#include "gaming_hub.h"

void	hub_load_settings(t_hub *hub)
{
	gchar	*path;

	(void)hub;
	path = g_build_filename(g_get_user_config_dir(),
				"gaming_hub", "settings.ini", NULL);
	g_print("%s\n", path);
	g_free(path);
}
