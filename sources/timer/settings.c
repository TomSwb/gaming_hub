/* ************************************************************************** */
/*                                                                            */
/*                                            :::::::    :::     :::  :::::   */
/*   settings.c                               :+:   :+:  :+:+: :+:+:   :+:    */
/*                                            +#++:+#+   +#+ +:+ +#+   +#+    */
/*   By: TomSwb <github.com/TomSwb>           +#+   +#+  +#+     +#+   +#+    */
/*                                            ########   ###     ###  #####   */
/*   Created: 2026/11/11 09:17:09 by TomSwb                                   */
/*   Updated: 2026/11/11 09:17:09 by TomSwb                   bitminded.ch    */
/*                                                                            */
/* ************************************************************************** */

#include "gaming_hub.h"

void	hub_load_settings(t_hub *hub)
{
	gchar		*path;
	GKeyFile	*key_file;

	path = g_build_filename(g_get_user_config_dir(),
				"gaming_hub", "settings.ini", NULL);
	key_file = g_key_file_new();
	if (hub_key_file_error(key_file, path))
		hub_set_settings(key_file, hub);
	g_key_file_free(key_file);
	g_free(path);
	g_print("session: %d\n wind down: %d\n break: %d\n eye break: %d\n eye cue: %d\n",
			hub->setting.session, hub->setting.wind_down, hub->setting.break_min, hub->setting.eye_min, hub->setting.eye_sec);
}

gboolean	hub_key_file_error(GKeyFile *key_file, gchar *path)
{
	GError	*error;

	error = NULL;
	if (!g_key_file_load_from_file(key_file, path, G_KEY_FILE_NONE, &error))
	{
		if (!g_error_matches(error, G_FILE_ERROR, G_FILE_ERROR_NOENT))
			g_printerr("Can't read file: %s, %s\n", path, error->message);
		g_error_free(error);
		return (FALSE);
	}
	return (TRUE);
}

void	hub_set_settings(GKeyFile *key_file, t_hub *hub)
{
	hub_read_setting(key_file, "timer",
		"session_minutes", &hub->setting.session);
	hub_read_setting(key_file, "timer",
		"wind_down_minutes", &hub->setting.wind_down);
	hub_read_setting(key_file, "timer",
		"break_every_minutes", &hub->setting.break_min);
	hub_read_setting(key_file, "timer",
		"eye_every_minutes", &hub->setting.eye_min);
	hub_read_setting(key_file, "timer",
		"eye_seconds", &hub->setting.eye_sec);
}

void	hub_read_setting(GKeyFile *key_file, const gchar *group,
			const gchar *key, gint *value)
{
	GError		*error;
	gint		temp;
	gboolean	missing;

	error = NULL;
	temp = g_key_file_get_integer(key_file, group, key, &error);
	missing = g_error_matches(error, G_KEY_FILE_ERROR,
				G_KEY_FILE_ERROR_GROUP_NOT_FOUND)
			|| g_error_matches(error, G_KEY_FILE_ERROR,
				G_KEY_FILE_ERROR_KEY_NOT_FOUND);
	if (error)
	{
		if (!missing)
			g_printerr("Can't read file: %s %s\n", key, error->message);
		g_error_free(error);
		return ;
	}
	if (temp <= 0)
	{
		g_printerr("Value must be higher than 0: %s - using default\n", key);
		return ;
	}
	*value = temp;
}
