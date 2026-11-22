/* ************************************************************************** */
/*                                                                            */
/*                                            :::::::    :::     :::  :::::   */
/*   bluez.c                                  :+:   :+:  :+:+: :+:+:   :+:    */
/*                                            +#++:+#+   +#+ +:+ +#+   +#+    */
/*   By: TomSwb <github.com/TomSwb>           +#+   +#+  +#+     +#+   +#+    */
/*                                            ########   ###     ###  #####   */
/*   Created: 2026/11/22 14:26:00 by TomSwb                                   */
/*   Updated: 2026/11/22 14:26:00 by TomSwb                   bitminded.ch    */
/*                                                                            */
/* ************************************************************************** */

#include "gaming_hub.h"

void	hub_bluez_connect(t_hub *hub)
{
	g_dbus_object_manager_client_new_for_bus(G_BUS_TYPE_SYSTEM,
		G_DBUS_OBJECT_MANAGER_CLIENT_FLAGS_NONE, HUB_BLUEZ_NAME,
		"/", NULL, NULL, NULL, NULL, hub_bluez_ready, hub);
}

void	hub_bluez_ready(GObject *source, GAsyncResult *res, gpointer user_data)
{
	t_hub				*hub;
	GDBusObjectManager	*manager;
	GError				*error;

	(void)source;
	hub = user_data;
	error = NULL;
	manager = g_dbus_object_manager_client_new_for_bus_finish(res, &error);
	if (!manager)
	{
		g_printerr("bluez: %s\n", error->message);
		g_error_free(error);
		return ;
	}
	hub->headset.obj_man = manager;
	g_print("bluez: ready\n");
}
