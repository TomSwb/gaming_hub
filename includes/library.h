/* ************************************************************************** */
/*                                                                            */
/*                                            :::::::    :::     :::  :::::   */
/*   library.h                                :+:   :+:  :+:+: :+:+:   :+:    */
/*                                            +#++:+#+   +#+ +:+ +#+   +#+    */
/*   By: TomSwb <github.com/TomSwb>           +#+   +#+  +#+     +#+   +#+    */
/*                                            ########   ###     ###  #####   */
/*   Created: 2026/11/23 10:06:23 by TomSwb                                   */
/*   Updated: 2026/11/23 10:06:23 by TomSwb                   bitminded.ch    */
/*                                                                            */
/* ************************************************************************** */

#ifndef LIBRARY_H
# define LIBRARY_H

// === Library === //

# include "structs.h"

// === Functions === //

// sources/library/

// hub_game.c
GType	hub_game_get_type(void);
void	hub_game_class_init(gpointer klass, gpointer data);
void	hub_game_init(GTypeInstance *instance, gpointer klass);

#endif
