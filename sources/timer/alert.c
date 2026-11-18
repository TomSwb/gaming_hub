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
