/* ************************************************************************** */
/*                                                                            */
/*                                            :::::::    :::     :::  :::::   */
/*   power.c                                  :+:   :+:  :+:+: :+:+:   :+:    */
/*                                            +#++:+#+   +#+ +:+ +#+   +#+    */
/*   By: TomSwb <github.com/TomSwb>           +#+   +#+  +#+     +#+   +#+    */
/*                                            ########   ###     ###  #####   */
/*   Created: 2026/11/22 09:48:18 by TomSwb                                   */
/*   Updated: 2026/11/22 09:48:18 by TomSwb                   bitminded.ch    */
/*                                                                            */
/* ************************************************************************** */

#include "gaming_hub.h"

void	hub_power_connect(t_hub *hub)
{
	g_dbus_proxy_new_for_bus(G_BUS_TYPE_SYSTEM, G_DBUS_PROXY_FLAGS_NONE, NULL,
			HUB_PPD_NAME, HUB_PPD_PATH, HUB_PPD_IFACE, NULL,
			hub_power_ready, hub);
}

void	hub_power_ready(GObject *source, GAsyncResult *res, gpointer user_data)
{
	t_hub		*hub;
	GDBusProxy	*result;
	GError		*error;

	(void)source;
	hub = user_data;
	error = NULL;
	result = g_dbus_proxy_new_for_bus_finish(res, &error);
	if (!result)
	{
		g_printerr("power: error: %s\n", error->message);
		g_error_free(error);
		return ;
	}
	hub->power.proxy = result;
	g_print("power: ready\n");
}
