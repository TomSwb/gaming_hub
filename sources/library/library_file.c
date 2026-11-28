/* ************************************************************************** */
/*                                                                            */
/*                                            :::::::    :::     :::  :::::   */
/*   library_file.c                           :+:   :+:  :+:+: :+:+:   :+:    */
/*                                            +#++:+#+   +#+ +:+ +#+   +#+    */
/*   By: TomSwb <github.com/TomSwb>           +#+   +#+  +#+     +#+   +#+    */
/*                                            ########   ###     ###  #####   */
/*   Created: 2026/11/28 10:06:57 by TomSwb                                   */
/*   Updated: 2026/11/28 10:06:57 by TomSwb                   bitminded.ch    */
/*                                                                            */
/* ************************************************************************** */

#include "gaming_hub.h"

void	hub_library_load(t_library *library)
{
	GError	*error;
	gchar	*broken;

	error = NULL;
	library->file_path = g_build_filename(g_get_user_data_dir(),
			HUB_DATA_DIR, HUB_LIBRARY_FILE, NULL);
	library->file = g_key_file_new();
	if (g_key_file_load_from_file(library->file, library->file_path,
			G_KEY_FILE_NONE, &error))
		return ;
	if (!g_error_matches(error, G_FILE_ERROR, G_FILE_ERROR_NOENT))
	{
		broken = g_strconcat(library->file_path, HUB_BROKEN_SUFFIX, NULL);
		g_printerr("%s: %s - kept as %s, starting fresh\n",
				library->file_path, error->message, broken);
		g_rename(library->file_path, broken);
		g_free(broken);
	}
	g_error_free(error);
	g_key_file_free(library->file);
	library->file = g_key_file_new();
}
