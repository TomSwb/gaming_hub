/* ************************************************************************** */
/*                                                                            */
/*                                            :::::::    :::     :::  :::::   */
/*   game.h                                   :+:   :+:  :+:+: :+:+:   :+:    */
/*                                            +#++:+#+   +#+ +:+ +#+   +#+    */
/*   By: TomSwb <github.com/TomSwb>           +#+   +#+  +#+     +#+   +#+    */
/*                                            ########   ###     ###  #####   */
/*   Created: 2026/11/20 20:06:28 by TomSwb                                   */
/*   Updated: 2026/11/20 20:06:28 by TomSwb                   bitminded.ch    */
/*                                                                            */
/* ************************************************************************** */

#ifndef GAME_H
# define GAME_H

// === Library === //

# include "structs.h"

// === Functions === //

// sources/game/

// game_events.c
void		hub_game_closed(t_hub *hub);
void		hub_game_started(t_hub *hub);

// launch.c
gboolean	hub_launch_steam(const t_hub_game *game);

#endif
