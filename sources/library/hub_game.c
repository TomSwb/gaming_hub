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
	(void)data;
	G_OBJECT_CLASS(klass)->finalize = hub_game_finalize;
}

void	hub_game_init(GTypeInstance *instance, gpointer klass)
{
	t_hub_game	*game;

	(void)klass;
	game = (t_hub_game *)instance;
	game->name = NULL;
	game->platform = NULL;
	game->id = NULL;
	game->path = NULL;
	game->source = GAME_MANUAL;
}

void	hub_game_finalize(GObject *object)
{
	t_hub_game		*game;
	GObjectClass	*parent;

	g_print("finalize\n");
	game = (t_hub_game *)object;
	g_free(game->name);
	g_free(game->platform);
	g_free(game->id);
	g_free(game->path);
	parent = g_type_class_peek_parent(G_OBJECT_GET_CLASS(object));
	parent->finalize(object);
}
