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

void	hub_library_fake(GListStore *store)
{
	t_game_info	info;

	info.name = "Portal";
	info.source = GAME_STEAM;
	info.platform = "Steam";
	info.id = "400";
	info.path = "/mnt/Steam2TB/SteamLibrary/steamapps/common/Portal";
	hub_library_add(store, &info);
	info.name = "Super Mario Galaxy";
	info.source = GAME_EMULATOR;
	info.platform = "Wii";
	info.id = "RMGE01";
	info.path = "/home/tomswb/Emulation/roms/wii/Super Mario Galaxy.iso";
	hub_library_add(store, &info);
	info.name = "Jak X";
	info.source = GAME_EMULATOR;
	info.platform = "PS2";
	info.id = "SCES-53286";
	info.path = "/home/tomswb/Emulation/roms/ps2/Jak X.iso";
	hub_library_add(store, &info);
}
