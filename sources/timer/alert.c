/* ************************************************************************** */
/*                                                                            */
/*                                            :::::::    :::     :::  :::::   */
/*   alert.c                                  :+:   :+:  :+:+: :+:+:   :+:    */
/*                                            +#++:+#+   +#+ +:+ +#+   +#+    */
/*   By: TomSwb <github.com/TomSwb>           +#+   +#+  +#+     +#+   +#+    */
/*                                            ########   ###     ###  #####   */
/*   Created: 2026/11/18 08:25:42 by TomSwb                                   */
/*   Updated: 2026/11/18 08:25:42 by TomSwb                   bitminded.ch    */
/*                                                                            */
/* ************************************************************************** */

#include "gaming_hub.h"

void	hub_notify(t_hub *hub, const gchar *id, const gchar *title,
			const gchar *body)
{
	GNotification	*n;

	n = g_notification_new(title);
	g_notification_set_body(n, body);
	g_application_send_notification(hub->app, id, n);
	g_object_unref(n);
}

void	hub_check_alerts(t_hub *hub, gint left)
{
	gchar	*text;

	text = g_strdup_printf("%d minutes left - head for a save point",
							hub->setting.wind_down);
	if (hub->timer.wind_warned == FALSE
		&& left <= hub->setting.wind_down * 60)
	{
		hub_notify(hub, "wind-down", "Wind down", text);
		hub->timer.wind_warned = TRUE;
	}
	g_free(text);
}
