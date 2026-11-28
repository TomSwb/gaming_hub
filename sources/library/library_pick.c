/* ************************************************************************** */
/*                                                                            */
/*                                            :::::::    :::     :::  :::::   */
/*   library_pick.c                           :+:   :+:  :+:+: :+:+:   :+:    */
/*                                            +#++:+#+   +#+ +:+ +#+   +#+    */
/*   By: TomSwb <github.com/TomSwb>           +#+   +#+  +#+     +#+   +#+    */
/*                                            ########   ###     ###  #####   */
/*   Created: 2026/11/24 21:17:05 by TomSwb                                   */
/*   Updated: 2026/11/24 21:17:05 by TomSwb                   bitminded.ch    */
/*                                                                            */
/* ************************************************************************** */

#include "gaming_hub.h"

void	hub_library_activate(GtkWidget *self, guint position,
			gpointer user_data)
{
	t_hub		*hub;
	t_hub_game	*game;

	(void)self;
	hub = user_data;
	game = hub_game_cast(g_list_model_get_item(G_LIST_MODEL(
					hub->library.selection), position));
	if (game->state != GAME_READY)
	{
		g_printerr(HUB_NOT_READY, game->name);
		g_object_unref(game);
		return ;
	}
	g_clear_object(&hub->library.picked);
	hub->library.picked = game;
	if (hub->timer.state == TIMER_TRANSIT)
	{
		if (hub_launch(hub))
			hub_game_started(hub);
		return ;
	}
	hub_library_plan(hub);
}

void	hub_library_plan(t_hub *hub)
{
	gchar	*text;

	text = g_strdup_printf("Game: %s", hub->library.picked->name);
	gtk_label_set_text(GTK_LABEL(hub->library.next), text);
	gtk_stack_set_visible_child_name(GTK_STACK(hub->stack), "plan");
	g_free(text);
}

GtkWidget	*hub_build_plan_page(t_hub *hub)
{
	GtkWidget	*page;
	GtkWidget	*row;
	GtkWidget	*back;

	page = gtk_box_new(GTK_ORIENTATION_VERTICAL, 6);
	row = gtk_center_box_new();
	back = gtk_button_new_with_label("Back to library");
	g_signal_connect(back, "clicked", G_CALLBACK(hub_library_back), hub);
	hub->library.next = gtk_label_new(NULL);
	gtk_center_box_set_start_widget(GTK_CENTER_BOX(row), back);
	gtk_center_box_set_center_widget(GTK_CENTER_BOX(row), hub->library.next);
	gtk_box_append(GTK_BOX(page), row);
	gtk_box_append(GTK_BOX(page), hub_build_plan(hub));
	return (page);
}

void	hub_library_back(GtkButton *self, gpointer user_data)
{
	t_hub	*hub;

	(void)self;
	hub = user_data;
	g_clear_object(&hub->library.picked);
	gtk_stack_set_visible_child_name(GTK_STACK(hub->stack), "library");
}
