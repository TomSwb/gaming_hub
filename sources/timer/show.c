/* ************************************************************************** */
/*                                                                            */
/*                                            :::::::    :::     :::  :::::   */
/*   show.c                                   :+:   :+:  :+:+: :+:+:   :+:    */
/*                                            +#++:+#+   +#+ +:+ +#+   +#+    */
/*   By: TomSwb <github.com/TomSwb>           +#+   +#+  +#+     +#+   +#+    */
/*                                            ########   ###     ###  #####   */
/*   Created: 2026/11/17 20:55:23 by TomSwb                                   */
/*   Updated: 2026/11/17 20:55:23 by TomSwb                   bitminded.ch    */
/*                                                                            */
/* ************************************************************************** */

#include "gaming_hub.h"

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

void	hub_show_eye(GtkWidget *label, gint seconds)
{
	gchar	*text;

	text = g_strdup_printf("Look at something 6m away for %d seconds", seconds);
	gtk_label_set_text(GTK_LABEL(label), text);
	g_free(text);
}
