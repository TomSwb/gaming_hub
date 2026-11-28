/* ************************************************************************** */
/*                                                                            */
/*                                            :::::::    :::     :::  :::::   */
/*   library_build.c                          :+:   :+:  :+:+: :+:+:   :+:    */
/*                                            +#++:+#+   +#+ +:+ +#+   +#+    */
/*   By: TomSwb <github.com/TomSwb>           +#+   +#+  +#+     +#+   +#+    */
/*                                            ########   ###     ###  #####   */
/*   Created: 2026/11/23 16:54:22 by TomSwb                                   */
/*   Updated: 2026/11/23 16:54:22 by TomSwb                   bitminded.ch    */
/*                                                                            */
/* ************************************************************************** */

#include "gaming_hub.h"

GtkWidget	*hub_build_library(t_hub *hub)
{
	GListStore			*store;
	GtkSingleSelection	*selection;
	GtkListItemFactory	*factory;
	GtkWidget			*view;
	GtkWidget			*scroll;

	store = g_list_store_new(hub_game_get_type());
	hub->library.games = store;
	hub_library_load(&hub->library);
	selection = gtk_single_selection_new(G_LIST_MODEL(store));
	factory = gtk_signal_list_item_factory_new();
	g_signal_connect(factory, "setup", G_CALLBACK(hub_row_setup), NULL);
	g_signal_connect(factory, "bind", G_CALLBACK(hub_row_bind), NULL);
	view = gtk_list_view_new(GTK_SELECTION_MODEL(selection), factory);
	g_signal_connect(view, "activate", G_CALLBACK(hub_library_activate), hub);
	scroll = gtk_scrolled_window_new();
	gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scroll), view);
	gtk_widget_set_vexpand(scroll, TRUE);
	return (scroll);
}

GtkWidget	*hub_build_library_page(t_hub *hub)
{
	GtkWidget	*page;
	GtkWidget	*refresh;

	page = gtk_box_new(GTK_ORIENTATION_VERTICAL, 6);
	refresh = gtk_button_new_with_label("Refresh");
	g_signal_connect(refresh, "clicked", G_CALLBACK(hub_library_refresh), hub);
	gtk_box_append(GTK_BOX(page), refresh);
	gtk_widget_set_halign(refresh, GTK_ALIGN_END);
	gtk_box_append(GTK_BOX(page), hub_build_library(hub));
	return (page);
}

void	hub_library_refresh(GtkButton *self, gpointer user_data)
{
	(void)self;
	hub_library_rescan(user_data);
}
