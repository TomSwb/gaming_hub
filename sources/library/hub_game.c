/* ************************************************************************** */
/*                                                                            */
/*                                            :::::::    :::     :::  :::::   */
/*   hub_game.c                               :+:   :+:  :+:+: :+:+:   :+:    */
/*                                            +#++:+#+   +#+ +:+ +#+   +#+    */
/*   By: TomSwb <github.com/TomSwb>           +#+   +#+  +#+     +#+   +#+    */
/*                                            ########   ###     ###  #####   */
/*   Created: 2026/11/23 10:34:32 by TomSwb                                   */
/*   Updated: 2026/11/23 10:34:32 by TomSwb                   bitminded.ch    */
/*                                                                            */
/* ************************************************************************** */

#include "gaming_hub.h"

GType	hub_game_get_type(void)
{
	static gsize	type_id;
	GType			result;

	if (g_once_init_enter(&type_id))
	{
		result = g_type_register_static_simple(G_TYPE_OBJECT,
				HUB_GAME_TYPE_NAME, sizeof(t_hub_game_class),
				hub_game_class_init, sizeof(t_hub_game),
				hub_game_init, 0);
		g_once_init_leave(&type_id, result);
	}
	return (type_id);
}

void	hub_game_class_init(gpointer klass, gpointer data)
{
	(void)klass;
	(void)data;
}

void	hub_game_init(GTypeInstance *instance, gpointer klass)
{
	(void)instance;
	(void)klass;
}
