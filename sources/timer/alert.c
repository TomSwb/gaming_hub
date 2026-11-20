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
	GNotification	*notif;

	notif = g_notification_new(title);
	g_notification_set_body(notif, body);
	g_application_send_notification(hub->app, id, notif);
	g_object_unref(notif);
}

void	hub_wind_down_alerts(t_hub *hub, gint left)
{
	gchar	*text;

	if (hub->timer.wind_warned == FALSE
		&& left <= hub->setting.wind_down * 60)
	{
		text = g_strdup_printf("%d minutes left - head for a save point",
				hub->setting.wind_down);
		hub_alert(hub, "wind-down", "Wind down", text);
		hub->timer.wind_warned = TRUE;
		g_free(text);
	}
}

void	hub_alert(t_hub *hub, const gchar *id, const gchar *title,
			const gchar *body)
{
	if (hub->timer.popups)
		hub_notify(hub, id, title, body);
	if (hub->timer.sound_on)
	{
		gtk_media_stream_seek(hub->timer.sound, 0);
		gtk_media_stream_play(hub->timer.sound);
	}
}
