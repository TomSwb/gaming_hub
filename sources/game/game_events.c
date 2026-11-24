/* ************************************************************************** */
/*                                                                            */
/*                                            :::::::    :::     :::  :::::   */
/*   game_events.c                            :+:   :+:  :+:+: :+:+:   :+:    */
/*                                            +#++:+#+   +#+ +:+ +#+   +#+    */
/*   By: TomSwb <github.com/TomSwb>           +#+   +#+  +#+     +#+   +#+    */
/*                                            ########   ###     ###  #####   */
/*   Created: 2026/11/20 20:04:36 by TomSwb                                   */
/*   Updated: 2026/11/20 20:04:36 by TomSwb                   bitminded.ch    */
/*                                                                            */
/* ************************************************************************** */

#include "gaming_hub.h"

void	hub_game_closed(t_hub *hub)
{
	if (hub->timer.state == TIMER_PLAYING || hub->timer.state == TIMER_PAUSED
		|| hub->timer.state == TIMER_BREAK)
	{
		if (hub->timer.state == TIMER_PAUSED)
			hub->timer.started += g_get_monotonic_time() - hub->timer.paused_at;
		hub->timer.state = TIMER_TRANSIT;
		gtk_button_set_label(GTK_BUTTON(hub->timer.pause), "Pause");
		gtk_widget_set_visible(hub->timer.eye, FALSE);
		hub_show_timer_page(hub);
	}
}

void	hub_game_started(t_hub *hub)
{
	gint64	elapsed;

	elapsed = (g_get_monotonic_time() - hub->timer.started) / G_USEC_PER_SEC;
	if (hub->timer.state == TIMER_TRANSIT)
	{
		hub->timer.state = TIMER_PLAYING;
		if (elapsed >= hub->timer.next_break)
			hub->timer.next_break = (gint)elapsed + hub->setting.break_min * 60;
		hub_show_timer_page(hub);
	}
	else if (hub->timer.state == TIMER_PLANNING
			|| hub->timer.state == TIMER_ENDED)
	{
		gtk_stack_set_visible_child_name(GTK_STACK(hub->stack), "plan");
		g_clear_object(&hub->library.picked);
		gtk_label_set_text(GTK_LABEL(hub->library.next), "");
	}
}
