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
	else if (hub->timer.state == TIMER_PLAYING
		|| hub->timer.state == TIMER_PAUSED
		|| hub->timer.state == TIMER_BREAK)
		hub_show_playing(hub, HUB_PLAYING);
}

void	hub_show_playing(t_hub *hub, const gchar *format)
{
	gchar	*text;

	if (!hub->library.picked)
	{
		gtk_label_set_text(GTK_LABEL(hub->library.playing), "");
		return ;
	}
	text = g_strdup_printf(format, hub->library.picked->name);
	gtk_label_set_text(GTK_LABEL(hub->library.playing), text);
	g_free(text);
}

void	hub_game_exited(GObject *source, GAsyncResult *res,
			gpointer user_data)
{
	t_hub		*hub;
	GSubprocess	*proc;
	GError		*error;

	hub = user_data;
	proc = G_SUBPROCESS(source);
	error = NULL;
	if (!g_subprocess_wait_finish(proc, res, &error))
	{
		g_printerr("%s\n", error->message);
		g_error_free(error);
		return ;
	}
	if (g_subprocess_get_if_exited(proc)
		&& g_subprocess_get_exit_status(proc) != 0)
		g_printerr("emulator ended with code %d\n",
			g_subprocess_get_exit_status(proc));
	if (proc == hub->library.running)
	{
		g_clear_object(&hub->library.running);
		hub_game_closed(hub);
	}
}
