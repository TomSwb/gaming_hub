/* ************************************************************************** */
/*                                                                            */
/*                                            :::::::    :::     :::  :::::   */
/*   inits_2.c                                :+:   :+:  :+:+: :+:+:   :+:    */
/*                                            +#++:+#+   +#+ +:+ +#+   +#+    */
/*   By: TomSwb <github.com/TomSwb>           +#+   +#+  +#+     +#+   +#+    */
/*                                            ########   ###     ###  #####   */
/*   Created: 2026/11/22 09:51:37 by TomSwb                                   */
/*   Updated: 2026/11/22 09:51:37 by TomSwb                   bitminded.ch    */
/*                                                                            */
/* ************************************************************************** */

#include "gaming_hub.h"

void	hub_power_init(t_hub *hub)
{
	hub->power.profile = NULL;
	hub->power.pow_but_row = NULL;
	hub->power.proxy = NULL;
}

void	hub_library_init(t_hub *hub)
{
	hub->library.games = NULL;
	hub->library.picked = NULL;
	hub->library.next = NULL;
	hub->library.playing = NULL;
	hub->library.running = NULL;
}
