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
	GError		*error;
	GKeyFile	*key_file;

	(void)hub;
	error = NULL;
	path = g_build_filename(g_get_user_config_dir(),
				"gaming_hub", "settings.ini", NULL);
	key_file = g_key_file_new();
	if (!hub_key_file_error(key_file, path, error))
		return ;
	if (!hub_read_setting(key_file, "timer",
			"session_minutes", &hub->setting.session))
	{
		g_free(path);
		return ;
	}
	g_key_file_free(key_file);
	g_print("%s\n", path);
	g_free(path);
}

gboolean	hub_key_file_error(GKeyFile *key_file, gchar *path, GError *error)
{
	if (!g_key_file_load_from_file(key_file, path, G_KEY_FILE_NONE, &error))
	{
		if (!g_error_matches(error, G_FILE_ERROR, G_FILE_ERROR_NOENT))
			g_printerr("Can't read file: %s, %s\n", path, error->message);
		g_error_free(error);
		g_key_file_free(key_file);
		g_free(path);
		return (FALSE);
	}
	return (TRUE);
}

gboolean	hub_read_setting(GKeyFile *key_file, const gchar *group,
			const gchar *key, gint *value)
{
	GError	*error;
	gint	temp;

	error = NULL;
	temp = g_key_file_get_integer(key_file, group, key, &error);
	if (error)
	{
		if (!g_error_matches(error, G_FILE_ERROR, G_FILE_ERROR_NOENT))
			g_printerr("Can't read file: %s %s\n", key, error->message);
		g_error_free(error);
		g_key_file_free(key_file);
		return (FALSE);
	}
	*value = temp;
	return (TRUE);
}
