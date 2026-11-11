/* ************************************************************************** */
/*                                                                            */
/*                                            :::::::    :::     :::  :::::   */
/*   session.c                                :+:   :+:  :+:+: :+:+:   :+:    */
/*                                            +#++:+#+   +#+ +:+ +#+   +#+    */
/*   By: TomSwb <github.com/TomSwb>           +#+   +#+  +#+     +#+   +#+    */
/*                                            ########   ###     ###  #####   */
/*   Created: 2026/11/11 13:57:11 by TomSwb                                   */
/*   Updated: 2026/11/11 13:57:11 by TomSwb                   bitminded.ch    */
/*                                                                            */
/* ************************************************************************** */

#include "gaming_hub.h"

void	hub_setting_timer(t_hub *hub)
{
	hub->timer.session = gtk_spin_button_new_with_range(HUB_SESSION_LOWEST,
			HUB_SESSION_HIGHEST, HUB_SESSION_STEP);
	gtk_spin_button_set_value(GTK_SPIN_BUTTON(hub->timer.session),
			hub->setting.session);
	hub->timer.start = gtk_button_new_with_label("Start");
	g_signal_connect(GTK_BUTTON(hub->timer.start),
		"clicked", G_CALLBACK(hub_start_clicked), hub);
}

void 	hub_start_clicked(GtkButton *self, gpointer user_data)
{
	t_hub	*hub;

	(void)self;
	hub = user_data;
	hub->timer.planned = gtk_spin_button_get_value_as_int(
			GTK_SPIN_BUTTON(hub->timer.session));
	g_print("%d\n", hub->timer.planned);
}
