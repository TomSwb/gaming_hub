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

void	hub_library_add(GListStore *store, const t_game_info *info)
{
	t_hub_game	*game;

	game = hub_game_new(info);
	g_list_store_append(store, game);
	g_object_unref(game);
}
