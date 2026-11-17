/* ************************************************************************** */
/*                                                                            */
/*                                            :::::::    :::     :::  :::::   */
/*   buttons.c                                :+:   :+:  :+:+: :+:+:   :+:    */
/*                                            +#++:+#+   +#+ +:+ +#+   +#+    */
/*   By: TomSwb <github.com/TomSwb>           +#+   +#+  +#+     +#+   +#+    */
/*                                            ########   ###     ###  #####   */
/*   Created: 2026/11/17 10:38:28 by TomSwb                                   */
/*   Updated: 2026/11/17 10:38:28 by TomSwb                   bitminded.ch    */
/*                                                                            */
/* ************************************************************************** */

#include "gaming_hub.h"

void	hub_start_clicked(GtkButton *self, gpointer user_data)
{
	t_hub	*hub;

	(void)self;
	hub = user_data;
	if (hub->timer.state == TIMER_PLANNING || hub->timer.state == TIMER_ENDED)
	{
		hub->timer.planned = gtk_spin_button_get_value_as_int(
				GTK_SPIN_BUTTON(hub->timer.session));
		hub->timer.state = TIMER_PLAYING;
		hub->timer.started = g_get_monotonic_time();
	}
}

void	hub_pause_clicked(GtkButton *self, gpointer user_data)
{
	t_hub	*hub;

	hub = user_data;
	if (hub->timer.state == TIMER_PLAYING)
	{
		hub->timer.paused_at = g_get_monotonic_time();
		hub->timer.state = TIMER_PAUSED;
		gtk_button_set_label(self, "Resume");
	}
	else if (hub->timer.state == TIMER_PAUSED)
	{
		hub->timer.started += g_get_monotonic_time() - hub->timer.paused_at;
		hub->timer.state = TIMER_PLAYING;
		gtk_button_set_label(self, "Pause");
	}
}

void	hub_stop_clicked(GtkButton *self, gpointer user_data)
{
	t_hub	*hub;

	(void)self;
	hub = user_data;
	if (hub->timer.state == TIMER_PLAYING || hub->timer.state == TIMER_PAUSED)
	{
		hub->timer.state = TIMER_ENDED;
		hub_show_time(hub->timer.countdown, 0);
		gtk_button_set_label(GTK_BUTTON(hub->timer.pause), "Pause");
	}
}
