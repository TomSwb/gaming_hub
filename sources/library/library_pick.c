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

void	hub_library_activate(GtkListView *self, guint position,
			gpointer user_data)
{
	t_hub		*hub;
	t_hub_game	*game;

	hub = user_data;
	game = hub_game_cast(g_list_model_get_item(G_LIST_MODEL(
					gtk_list_view_get_model(self)), position));
	g_clear_object(&hub->library.picked);
	hub->library.picked = game;
	g_print("picked: %s\n", hub->library.picked->name);
	gtk_stack_set_visible_child_name(GTK_STACK(hub->stack), "plan");
}
