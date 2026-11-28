/* ************************************************************************** */
/*                                                                            */
/*                                            :::::::    :::     :::  :::::   */
/*   library_store.c                          :+:   :+:  :+:+: :+:+:   :+:    */
/*                                            +#++:+#+   +#+ +:+ +#+   +#+    */
/*   By: TomSwb <github.com/TomSwb>           +#+   +#+  +#+     +#+   +#+    */
/*                                            ########   ###     ###  #####   */
/*   Created: 2026/11/23 16:16:48 by TomSwb                                   */
/*   Updated: 2026/11/23 16:16:48 by TomSwb                   bitminded.ch    */
/*                                                                            */
/* ************************************************************************** */

#include "gaming_hub.h"

void	hub_library_rescan(t_hub *hub)
{
	hub_steam_scan(hub->library.file);
	hub_platforms_scan(hub->library.file);
	hub_library_fill(&hub->library);
	hub_library_check(&hub->library);
	hub_library_save(&hub->library);
	hub_library_reselect(&hub->library);
}

void	hub_library_reselect(t_library *library)
{
	GListModel	*games;
	guint		n;
	guint		i;
	t_hub_game	*game;

	games = G_LIST_MODEL(library->selection);
	n = g_list_model_get_n_items(games);
	i = 0;
	while (library->last && i < n)
	{
		game = hub_game_cast(g_list_model_get_item(games, i));
		if (hub_game_same(game, library->last))
			gtk_single_selection_set_selected(library->selection, i);
		g_object_unref(game);
		i++;
	}
	hub_view_focus(library);
}
