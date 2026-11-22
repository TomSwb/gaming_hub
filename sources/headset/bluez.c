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
	GDBusObject			*device;

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
	hub->headset.bt_add = g_strdup("54:4C:8A:E1:83:E0");
	device = hub_bluez_find(hub);
	if (device)
	{
		g_print("%s\n", g_dbus_object_get_object_path(device));
		g_object_unref(device);
	}
	else
		g_print("bluez; no found");
}

gboolean	hub_bluez_has_address(GDBusObject *object, const gchar *address)
{
	GDBusInterface	*device;
	GVariant		*value;
	gboolean		found;

	device = g_dbus_object_get_interface(object, HUB_BLUEZ_DEVICE);
	if (!device)
		return (FALSE);
	value = g_dbus_proxy_get_cached_property(G_DBUS_PROXY(device), "Address");
	found = value && g_variant_is_of_type(value, G_VARIANT_TYPE_STRING)
		&& g_strcmp0(g_variant_get_string(value, NULL), address) == 0;
	if (value)
		g_variant_unref(value);
	g_object_unref(device);
	return (found);
}

GDBusObject	*hub_bluez_find(t_hub *hub)
{
	GList		*node;
	GDBusObject	*found;

	if (!hub->headset.obj_man || !hub->headset.bt_add)
		return (NULL);
	found = NULL;
	node = g_dbus_object_manager_get_objects(hub->headset.obj_man);
	while (node && !found)
	{
		if (hub_bluez_has_address(G_DBUS_OBJECT(node->data),
			hub->headset.bt_add))
				found = g_object_ref(node->data);
		node = node->next;
	}
	g_list_free_full(node, g_object_unref);
	return (found);
}
