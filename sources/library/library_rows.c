/* ************************************************************************** */
/*                                                                            */
/*                                            :::::::    :::     :::  :::::   */
/*   library_rows.c                           :+:   :+:  :+:+: :+:+:   :+:    */
/*                                            +#++:+#+   +#+ +:+ +#+   +#+    */
/*   By: TomSwb <github.com/TomSwb>           +#+   +#+  +#+     +#+   +#+    */
/*                                            ########   ###     ###  #####   */
/*   Created: 2026/11/23 16:27:16 by TomSwb                                   */
/*   Updated: 2026/11/23 16:27:16 by TomSwb                   bitminded.ch    */
/*                                                                            */
/* ************************************************************************** */

#include "gaming_hub.h"

void	hub_row_setup(GtkSignalListItemFactory *self, GObject *object,
			gpointer user_data)
{
	GtkWidget	*label;
	GtkWidget	*row;

	(void)self;
	(void)user_data;
	label = gtk_label_new(NULL);
	row = hub_labelled(NULL, label);
	gtk_list_item_set_child(GTK_LIST_ITEM(object), row);
}

void	hub_row_bind(GtkSignalListItemFactory *self, GObject *object,
			gpointer user_data)
{
	GtkListItem	*item;
	t_hub_game	*game;
	GtkWidget	*row;

	(void)self;
	(void)user_data;
	item = GTK_LIST_ITEM(object);
	game = hub_game_cast(gtk_list_item_get_item(item));
	row = gtk_list_item_get_child(item);
	gtk_label_set_text(GTK_LABEL(gtk_widget_get_first_child(row)),
		game->name);
	gtk_label_set_text(GTK_LABEL(gtk_widget_get_last_child(row)),
		game->platform);
}
