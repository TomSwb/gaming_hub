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
