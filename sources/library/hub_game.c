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
	GObjectClass	*object_class;

	(void)data;
	object_class = G_OBJECT_CLASS(klass);
	object_class->finalize = hub_game_finalize;
	object_class->set_property = hub_game_set_property;
	object_class->get_property = hub_game_get_property;
	hub_game_install_props(object_class);
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
	game->command = NULL;
	game->state = GAME_READY;
	game->drive = NULL;
}

void	hub_game_finalize(GObject *object)
{
	t_hub_game		*game;
	GObjectClass	*parent;

	game = (t_hub_game *)object;
	g_free(game->name);
	g_free(game->platform);
	g_free(game->id);
	g_free(game->path);
	g_free(game->command);
	g_free(game->drive);
	parent = g_type_class_peek_parent(G_OBJECT_GET_CLASS(object));
	parent->finalize(object);
}

t_hub_game	*hub_game_new(const t_game_info *info)
{
	return (g_object_new(hub_game_get_type(),
			HUB_GAME_NAME, info->name,
			HUB_GAME_SOURCE, info->source,
			HUB_GAME_PLATFORM, info->platform,
			HUB_GAME_ID, info->id,
			HUB_GAME_PATH, info->path,
			HUB_GAME_COMMAND, info->command,
			NULL));
}
