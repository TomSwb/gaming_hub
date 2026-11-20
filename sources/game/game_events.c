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
