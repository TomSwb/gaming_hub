/* ************************************************************************** */
/*                                                                            */
/*                                            :::::::    :::     :::  :::::   */
/*   hub_game_props.c                         :+:   :+:  :+:+: :+:+:   :+:    */
/*                                            +#++:+#+   +#+ +:+ +#+   +#+    */
/*   By: TomSwb <github.com/TomSwb>           +#+   +#+  +#+     +#+   +#+    */
/*                                            ########   ###     ###  #####   */
/*   Created: 2026/11/23 13:44:29 by TomSwb                                   */
/*   Updated: 2026/11/23 13:44:29 by TomSwb                   bitminded.ch    */
/*                                                                            */
/* ************************************************************************** */

#include "gaming_hub.h"

void	hub_game_install_props(GObjectClass *object_class)
{
	GParamSpec	*props[GAME_PROP_COUNT];
	GParamFlags	flags;

	flags = G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS;
	props[GAME_PROP_NONE] = NULL;
	props[GAME_PROP_NAME] = g_param_spec_string(HUB_GAME_NAME, NULL, NULL,
			NULL, flags);
	props[GAME_PROP_SOURCE] = g_param_spec_int(HUB_GAME_SOURCE, NULL, NULL,
			GAME_STEAM, GAME_MANUAL, GAME_MANUAL, flags);
	props[GAME_PROP_PLATFORM] = g_param_spec_string(HUB_GAME_PLATFORM, NULL,
			NULL, NULL, flags);
	props[GAME_PROP_ID] = g_param_spec_string(HUB_GAME_ID, NULL, NULL,
			NULL, flags);
	props[GAME_PROP_COMMAND] = g_param_spec_string(HUB_GAME_COMMAND, NULL, NULL,
			NULL, flags);
	props[GAME_PROP_PATH] = g_param_spec_string(HUB_GAME_PATH, NULL, NULL,
			NULL, flags);
	props[GAME_PROP_STATE] = g_param_spec_int(HUB_GAME_STATE, NULL, NULL,
			GAME_READY, GAME_NOT_INSTALLED, GAME_READY,
			G_PARAM_READABLE | G_PARAM_STATIC_STRINGS);
	g_object_class_install_properties(object_class, GAME_PROP_COUNT, props);
}

void	hub_game_set_property(GObject *object, guint prop_id,
			const GValue *value, GParamSpec *pspec)
{
	t_hub_game	*game;

	game = (t_hub_game *)object;
	if (prop_id == GAME_PROP_NAME)
		hub_keep(&game->name, g_value_get_string(value));
	else if (prop_id == GAME_PROP_SOURCE)
		game->source = g_value_get_int(value);
	else if (prop_id == GAME_PROP_PLATFORM)
		hub_keep(&game->platform, g_value_get_string(value));
	else if (prop_id == GAME_PROP_ID)
		hub_keep(&game->id, g_value_get_string(value));
	else if (prop_id == GAME_PROP_COMMAND)
		hub_keep(&game->command, g_value_get_string(value));
	else if (prop_id == GAME_PROP_PATH)
		hub_keep(&game->path, g_value_get_string(value));
	else
		G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
}

void	hub_game_get_property(GObject *object, guint prop_id,
			GValue *value, GParamSpec *pspec)
{
	t_hub_game	*game;

	game = (t_hub_game *)object;
	if (prop_id == GAME_PROP_NAME)
		g_value_set_string(value, game->name);
	else if (prop_id == GAME_PROP_SOURCE)
		g_value_set_int(value, game->source);
	else if (prop_id == GAME_PROP_PLATFORM)
		g_value_set_string(value, game->platform);
	else if (prop_id == GAME_PROP_ID)
		g_value_set_string(value, game->id);
	else if (prop_id == GAME_PROP_COMMAND)
		g_value_set_string(value, game->command);
	else if (prop_id == GAME_PROP_PATH)
		g_value_set_string(value, game->path);
	else if (prop_id == GAME_PROP_STATE)
		g_value_set_int(value, game->state);
	else
		G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
}

t_hub_game	*hub_game_cast(gpointer object)
{
	return (G_TYPE_CHECK_INSTANCE_CAST(object,
			hub_game_get_type(), t_hub_game));
}

gboolean	hub_game_same(const t_hub_game *game, const t_hub_game *other)
{
	const gchar	*mine;
	const gchar	*theirs;

	if (game->source != other->source)
		return (FALSE);
	mine = game->id;
	if (mine == NULL)
		mine = game->name;
	theirs = other->id;
	if (theirs == NULL)
		theirs = other->name;
	return (g_strcmp0(mine, theirs) == 0);
}
