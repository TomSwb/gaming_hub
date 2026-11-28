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

	if (hub->library.picked && hub->library.picked->state != GAME_READY)
	{
		g_printerr(HUB_NOT_READY, hub->library.picked->name);
		return (FALSE);
	}
	launch = TRUE;
	if (hub->library.picked && hub->library.picked->source == GAME_STEAM)
		launch = hub_launch_steam(hub->library.picked);
	else if (hub->library.picked)
		launch = hub_launch_emulator(hub);
	if (launch)
		hub_show_playing(hub, HUB_STARTING);
	return (launch);
}

gboolean	hub_launch_emulator(t_hub *hub)
{
	gchar		**argv;
	GError		*error;
	GSubprocess	*proc;

	error = NULL;
	argv = hub_launch_argv(hub->library.picked);
	if (!argv)
		return (FALSE);
	proc = g_subprocess_newv((const gchar * const *)argv,
			G_SUBPROCESS_FLAGS_NONE, &error);
	g_strfreev(argv);
	if (!proc)
	{
		g_printerr("%s: %s\n", hub->library.picked->name, error->message);
		g_error_free(error);
		return (FALSE);
	}
	g_clear_object(&hub->library.running);
	hub->library.running = proc;
	g_subprocess_wait_async(proc, NULL, hub_game_exited, hub);
	return (TRUE);
}
