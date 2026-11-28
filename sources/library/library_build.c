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
	GListStore	*store;
	GtkWidget	*stack;

	store = g_list_store_new(hub_game_get_type());
	hub->library.games = store;
	hub_library_load(&hub->library);
	hub_drives_watch(hub);
	hub->library.selection = gtk_single_selection_new(G_LIST_MODEL(store));
	stack = gtk_stack_new();
	hub->library.views = stack;
	gtk_stack_add_titled(GTK_STACK(stack), hub_build_list(hub),
		HUB_VIEW_LIST, "List");
	gtk_stack_add_titled(GTK_STACK(stack), hub_build_grid(hub),
		HUB_VIEW_GRID, "Grid");
	hub_view_load(&hub->library);
	g_signal_connect(stack, HUB_NOTIFY_VIEW,
		G_CALLBACK(hub_view_changed), &hub->library);
	return (stack);
}

GtkWidget	*hub_build_list(t_hub *hub)
{
	GtkListItemFactory	*factory;
	GtkWidget			*view;
	GtkWidget			*scroll;

	factory = gtk_signal_list_item_factory_new();
	g_signal_connect(factory, "setup", G_CALLBACK(hub_row_setup), NULL);
	g_signal_connect(factory, "bind", G_CALLBACK(hub_row_bind), NULL);
	g_signal_connect(factory, "unbind", G_CALLBACK(hub_item_unbind), NULL);
	view = gtk_list_view_new(GTK_SELECTION_MODEL(hub->library.selection),
			factory);
	g_signal_connect(view, "activate", G_CALLBACK(hub_library_activate), hub);
	scroll = gtk_scrolled_window_new();
	gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scroll), view);
	gtk_widget_set_vexpand(scroll, TRUE);
	return (scroll);
}

GtkWidget	*hub_build_library_page(t_hub *hub)
{
	GtkWidget	*page;
	GtkWidget	*row;
	GtkWidget	*switcher;
	GtkWidget	*refresh;

	page = gtk_box_new(GTK_ORIENTATION_VERTICAL, 6);
	row = gtk_center_box_new();
	switcher = gtk_stack_switcher_new();
	refresh = gtk_button_new_with_label("Refresh");
	g_signal_connect(refresh, "clicked", G_CALLBACK(hub_library_refresh), hub);
	gtk_center_box_set_start_widget(GTK_CENTER_BOX(row), switcher);
	gtk_center_box_set_end_widget(GTK_CENTER_BOX(row), refresh);
	gtk_box_append(GTK_BOX(page), row);
	gtk_box_append(GTK_BOX(page), hub_build_library(hub));
	gtk_stack_switcher_set_stack(GTK_STACK_SWITCHER(switcher),
		GTK_STACK(hub->library.views));
	return (page);
}

void	hub_library_refresh(GtkButton *self, gpointer user_data)
{
	(void)self;
	hub_library_rescan(user_data);
}
