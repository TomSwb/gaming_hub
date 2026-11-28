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
	hub_library_save(&hub->library);
}
