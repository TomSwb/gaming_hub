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

gboolean	hub_launch(t_hub *hub)
{
	gboolean	launch;

	launch = TRUE;
	if (hub->library.picked && hub->library.picked->source == GAME_STEAM)
		launch = hub_launch_steam(hub->library.picked);
	else if (hub->library.picked)
	{
		g_printerr("%s: can't launch this game yet\n",
			hub->library.picked->name);
		launch = FALSE;
	}
	if (launch)
		hub_show_playing(hub, HUB_STARTING);
	return (launch);
}
