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
	g_signal_connect(hub->timer.start, "clicked",
			G_CALLBACK(hub_start_clicked), hub);
	hub->timer.countdown = gtk_label_new("Not Started");
	hub->timer.pause = gtk_button_new_with_label("Pause");
	g_signal_connect(hub->timer.pause, "clicked",
			G_CALLBACK(hub_pause_clicked), hub);
	hub->timer.stop = gtk_button_new_with_label("End Session");
	g_signal_connect(hub->timer.stop, "clicked",
			G_CALLBACK(hub_stop_clicked), hub);
	hub->timer.done = gtk_button_new_with_label("Done");
	g_signal_connect(hub->timer.done, "clicked",
			G_CALLBACK(hub_done_clicked), hub);
	hub->timer.eye = gtk_label_new("Look at something 6 m away for 20 seconds");
	gtk_widget_set_visible(hub->timer.eye, FALSE);
}

void	hub_update_timer(t_hub *hub)
{
	gint64	elapsed;
	gint	left;

	if (hub->timer.state != TIMER_PLAYING && hub->timer.state != TIMER_BREAK)
		return ;
	elapsed = (g_get_monotonic_time() - hub->timer.started) / G_USEC_PER_SEC;
	if (hub->timer.state == TIMER_PLAYING && elapsed >= hub->timer.next_break)
	{
		hub->timer.state = TIMER_BREAK;
		hub_show_timer_page(hub);
	}
	if (hub->timer.state == TIMER_PLAYING)
		hub_update_eye(hub, (gint)elapsed);
	left = (hub->timer.planned * 60) - (gint)elapsed;
	if (left <= 0)
	{
		left = 0;
		hub->timer.state = TIMER_ENDED;
		hub_show_timer_page(hub);
	}
	hub_show_time(hub->timer.countdown, left);
}

void	hub_show_time(GtkWidget *label, gint seconds)
{
	gint	hours;
	gint	minutes;
	gchar	*text;

	hours = seconds / 3600;
	minutes = (seconds % 3600) / 60;
	seconds = seconds % 60;
	text = g_strdup_printf("%d:%02d:%02d", hours, minutes, seconds);
	gtk_label_set_text(GTK_LABEL(label), text);
	g_free(text);
}

void	hub_show_timer_page(t_hub *hub)
{
	gboolean	planning;
	gboolean	playing;

	planning = hub->timer.state == TIMER_PLANNING
		|| hub->timer.state == TIMER_ENDED;
	playing = hub->timer.state == TIMER_PLAYING
		|| hub->timer.state == TIMER_PAUSED;
	if (planning)
		gtk_stack_set_visible_child_name(GTK_STACK(hub->stack), "plan");
	else if (playing)
		gtk_stack_set_visible_child_name(GTK_STACK(hub->stack), "session");
	else if (hub->timer.state == TIMER_BREAK)
		gtk_stack_set_visible_child_name(GTK_STACK(hub->stack), "break");
}

void	hub_update_eye(t_hub *hub, gint elapsed)
{
	if (elapsed >= hub->timer.next_eye)
	{
		gtk_widget_set_visible(hub->timer.eye, TRUE);
		hub->timer.eye_until = elapsed + hub->setting.eye_sec;
		hub->timer.next_eye += hub->setting.eye_min * 60;
	}
	if (elapsed >= hub->timer.eye_until)
		gtk_widget_set_visible(hub->timer.eye, FALSE);
}
