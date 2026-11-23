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
GType		hub_game_get_type(void);
void		hub_game_class_init(gpointer klass, gpointer data);
void		hub_game_init(GTypeInstance *instance, gpointer klass);
void		hub_game_finalize(GObject *object);
t_hub_game	*hub_game_new(const t_game_info *info);

// hub_game_props.c
void		hub_game_install_props(GObjectClass *object_class);
void		hub_game_set_property(GObject *object, guint prop_id,
				const GValue *value, GParamSpec *pspec);
void		hub_game_get_property(GObject *object, guint prop_id,
				GValue *value, GParamSpec *pspec);
t_hub_game	*hub_game_cast(gpointer object);

#endif
