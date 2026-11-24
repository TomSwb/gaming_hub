/* ************************************************************************** */
/*                                                                            */
/*                                            :::::::    :::     :::  :::::   */
/*   launch.c                                 :+:   :+:  :+:+: :+:+:   :+:    */
/*                                            +#++:+#+   +#+ +:+ +#+   +#+    */
/*   By: TomSwb <github.com/TomSwb>           +#+   +#+  +#+     +#+   +#+    */
/*                                            ########   ###     ###  #####   */
/*   Created: 2026/11/24 22:29:01 by TomSwb                                   */
/*   Updated: 2026/11/24 22:29:01 by TomSwb                   bitminded.ch    */
/*                                                                            */
/* ************************************************************************** */

#include "gaming_hub.h"

gboolean	hub_launch_steam(const t_hub_game *game)
{
	gchar		*uri;
	GError		*error;
	gboolean	launch;

	error = NULL;
	uri = g_strdup_printf(HUB_STEAM_RUN, game->id);
	launch = g_app_info_launch_default_for_uri(uri, NULL, &error);
	if (!launch)
	{
		g_printerr("%s, game: %s\n", error->message, game->name);
		g_error_free(error);
	}
	g_free(uri);
	return (launch);
}
