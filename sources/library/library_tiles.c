/* ************************************************************************** */
/*                                                                            */
/*                                            :::::::    :::     :::  :::::   */
/*   library_tiles.c                          :+:   :+:  :+:+: :+:+:   :+:    */
/*                                            +#++:+#+   +#+ +:+ +#+   +#+    */
/*   By: TomSwb <github.com/TomSwb>           +#+   +#+  +#+     +#+   +#+    */
/*                                            ########   ###     ###  #####   */
/*   Created: 2026/11/28 17:06:04 by TomSwb                                   */
/*   Updated: 2026/11/28 17:06:04 by TomSwb                   bitminded.ch    */
/*                                                                            */
/* ************************************************************************** */

#include "gaming_hub.h"

GtkWidget	*hub_tile_slot(void)
{
	GtkWidget	*slot;
	GtkWidget	*image;

	slot = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
	gtk_widget_set_size_request(slot, HUB_TILE_WIDTH, HUB_TILE_HEIGHT);
	gtk_widget_add_css_class(slot, HUB_TILE_CSS);
	image = gtk_image_new();
	gtk_image_set_pixel_size(GTK_IMAGE(image), HUB_TILE_ICON);
	gtk_widget_set_vexpand(image, TRUE);
	gtk_box_append(GTK_BOX(slot), image);
	return (slot);
}

void	hub_tile_setup(GtkSignalListItemFactory *self, GObject *object,
			gpointer user_data)
{
	GtkWidget	*tile;
	GtkWidget	*name;

	(void)self;
	(void)user_data;
	tile = gtk_box_new(GTK_ORIENTATION_VERTICAL, 4);
	gtk_widget_set_halign(tile, GTK_ALIGN_CENTER);
	gtk_box_append(GTK_BOX(tile), hub_tile_slot());
	name = gtk_label_new(NULL);
	gtk_label_set_wrap(GTK_LABEL(name), TRUE);
	gtk_label_set_lines(GTK_LABEL(name), HUB_TILE_LINES);
	gtk_label_set_ellipsize(GTK_LABEL(name), PANGO_ELLIPSIZE_END);
	gtk_label_set_max_width_chars(GTK_LABEL(name), HUB_TILE_CHARS);
	gtk_label_set_justify(GTK_LABEL(name), GTK_JUSTIFY_CENTER);
	gtk_box_append(GTK_BOX(tile), name);
	gtk_box_append(GTK_BOX(tile), gtk_label_new(NULL));
	gtk_box_append(GTK_BOX(tile), gtk_label_new(NULL));
	gtk_list_item_set_child(GTK_LIST_ITEM(object), tile);
}

void	hub_tile_bind(GtkSignalListItemFactory *self, GObject *object,
			gpointer user_data)
{
	t_hub_game	*game;
	GtkWidget	*tile;
	GtkWidget	*name;
	GtkWidget	*image;

	(void)self;
	(void)user_data;
	game = hub_game_cast(gtk_list_item_get_item(GTK_LIST_ITEM(object)));
	tile = gtk_list_item_get_child(GTK_LIST_ITEM(object));
	image = gtk_widget_get_first_child(gtk_widget_get_first_child(tile));
	name = gtk_widget_get_next_sibling(gtk_widget_get_first_child(tile));
	if (game->source == GAME_STEAM)
		gtk_image_set_from_icon_name(GTK_IMAGE(image), HUB_ICON_STEAM);
	else
		gtk_image_set_from_icon_name(GTK_IMAGE(image), HUB_ICON_GAME);
	gtk_label_set_text(GTK_LABEL(name), game->name);
	gtk_label_set_text(GTK_LABEL(gtk_widget_get_next_sibling(name)),
		game->platform);
	hub_item_state(tile, game);
	g_signal_connect(game, HUB_NOTIFY_STATE,
		G_CALLBACK(hub_item_notify), tile);
}

GtkWidget	*hub_build_grid(t_hub *hub)
{
	GtkListItemFactory	*factory;
	GtkWidget			*view;
	GtkWidget			*scroll;

	factory = gtk_signal_list_item_factory_new();
	g_signal_connect(factory, "setup", G_CALLBACK(hub_tile_setup), NULL);
	g_signal_connect(factory, "bind", G_CALLBACK(hub_tile_bind), NULL);
	g_signal_connect(factory, "unbind", G_CALLBACK(hub_item_unbind), NULL);
	g_object_ref(hub->library.selection);
	view = gtk_grid_view_new(GTK_SELECTION_MODEL(hub->library.selection),
			factory);
	gtk_grid_view_set_max_columns(GTK_GRID_VIEW(view), HUB_GRID_COLUMNS);
	g_signal_connect(view, "activate", G_CALLBACK(hub_library_activate), hub);
	scroll = gtk_scrolled_window_new();
	gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scroll), view);
	gtk_widget_set_vexpand(scroll, TRUE);
	return (scroll);
}
