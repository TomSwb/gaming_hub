/* ************************************************************************** */
/*                                                                            */
/*                                            :::::::    :::     :::  :::::   */
/*   library_view.c                           :+:   :+:  :+:+: :+:+:   :+:    */
/*                                            +#++:+#+   +#+ +:+ +#+   +#+    */
/*   By: TomSwb <github.com/TomSwb>           +#+   +#+  +#+     +#+   +#+    */
/*                                            ########   ###     ###  #####   */
/*   Created: 2026/11/28 17:52:06 by TomSwb                                   */
/*   Updated: 2026/11/28 17:52:06 by TomSwb                   bitminded.ch    */
/*                                                                            */
/* ************************************************************************** */

#include "gaming_hub.h"

void	hub_view_focus(t_library *library)
{
	GtkWidget	*view;
	guint		position;

	view = gtk_scrolled_window_get_child(GTK_SCROLLED_WINDOW(
				gtk_stack_get_visible_child(GTK_STACK(library->views))));
	position = gtk_single_selection_get_selected(library->selection);
	if (position == GTK_INVALID_LIST_POSITION)
		gtk_widget_grab_focus(view);
	else if (GTK_IS_GRID_VIEW(view))
		gtk_grid_view_scroll_to(GTK_GRID_VIEW(view), position,
			GTK_LIST_SCROLL_FOCUS, NULL);
	else
		gtk_list_view_scroll_to(GTK_LIST_VIEW(view), position,
			GTK_LIST_SCROLL_FOCUS, NULL);
}

void	hub_view_save(const gchar *view)
{
	gchar		*path;
	GKeyFile	*file;
	GError		*error;

	path = g_build_filename(g_get_user_config_dir(), HUB_CONFIG_DIR,
			HUB_SETTINGS_FILE, NULL);
	file = g_key_file_new();
	error = NULL;
	if (g_key_file_load_from_file(file, path, G_KEY_FILE_KEEP_COMMENTS,
			&error) || g_error_matches(error, G_FILE_ERROR, G_FILE_ERROR_NOENT))
	{
		g_key_file_set_string(file, HUB_VIEW_GROUP, HUB_VIEW_KEY, view);
		g_clear_error(&error);
		g_key_file_save_to_file(file, path, &error);
	}
	if (error)
		g_printerr("%s: %s - view not saved\n", path, error->message);
	g_clear_error(&error);
	g_key_file_free(file);
	g_free(path);
}

void	hub_view_changed(GObject *self, GParamSpec *pspec, gpointer user_data)
{
	(void)pspec;
	hub_view_save(gtk_stack_get_visible_child_name(GTK_STACK(self)));
	hub_view_focus(user_data);
}
