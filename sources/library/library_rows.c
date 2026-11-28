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
	gtk_box_append(GTK_BOX(row), gtk_label_new(NULL));
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
	gtk_label_set_text(GTK_LABEL(gtk_widget_get_next_sibling(
		gtk_widget_get_first_child(row))), game->platform);
	hub_row_state(row, game);
}

void	hub_row_state(GtkWidget *row, const t_hub_game *game)
{
	GtkWidget	*label;
	gchar		*text;

	label = gtk_widget_get_last_child(row);
	gtk_widget_set_opacity(row, 1.0);
	if (game->state == GAME_READY)
	{
		gtk_label_set_text(GTK_LABEL(label), "");
		return ;
	}
	gtk_widget_set_opacity(row, HUB_ROW_DIM);
	if (game->state == GAME_NOT_INSTALLED)
	{
		gtk_label_set_text(GTK_LABEL(label), HUB_NOT_INSTALLED);
		return ;
	}
	text = g_strdup_printf(HUB_PLUG_IN, ft_strrchr(game->drive, '/') + 1);
	gtk_label_set_text(GTK_LABEL(label), text);
	g_free(text);
}
