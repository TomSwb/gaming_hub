/* ************************************************************************** */
/*                                                                            */
/*                                            :::::::    :::     :::  :::::   */
/*   bluez_show.c                             :+:   :+:  :+:+: :+:+:   :+:    */
/*                                            +#++:+#+   +#+ +:+ +#+   +#+    */
/*   By: TomSwb <github.com/TomSwb>           +#+   +#+  +#+     +#+   +#+    */
/*                                            ########   ###     ###  #####   */
/*   Created: 2026/11/22 16:36:51 by TomSwb                                   */
/*   Updated: 2026/11/22 16:36:51 by TomSwb                   bitminded.ch    */
/*                                                                            */
/* ************************************************************************** */

#include "gaming_hub.h"

GVariant	*hub_bluez_prop(GDBusObject *object, const gchar *iface,
				const gchar *prop)
{
	GDBusInterface	*interface;
	GVariant		*value;

	interface = g_dbus_object_get_interface(object, iface);
	if (!interface)
		return (NULL);
	value = g_dbus_proxy_get_cached_property(G_DBUS_PROXY(interface), prop);
	g_object_unref(interface);
	return (value);
}

void	hub_bluez_show(t_hub *hub)
{
	GDBusObject	*device;
	gchar		*text;

	text = NULL;
	device = hub_bluez_find(hub);
	if (device)
	{
		text = hub_bluez_text(device);
		g_object_unref(device);
	}
	if (text)
		gtk_label_set_text(GTK_LABEL(hub->headset.headset_data), text);
	else if (hub->headset.outp_name)
		gtk_label_set_text(GTK_LABEL(hub->headset.headset_data),
			hub->headset.outp_name);
	else
		gtk_label_set_text(GTK_LABEL(hub->headset.headset_data), "...");
	g_free(text);
}

gchar	*hub_bluez_text(GDBusObject *device)
{
	GVariant	*value;
	gboolean	connected;
	gchar		*text;

	value = hub_bluez_prop(device, HUB_BLUEZ_DEVICE, "Connected");
	connected = value && g_variant_is_of_type(value, G_VARIANT_TYPE_BOOLEAN)
		&& g_variant_get_boolean(value);
	if (value != NULL)
		g_variant_unref(value);
	if (!connected)
		return (NULL);
	value = hub_bluez_prop(device, HUB_BLUEZ_DEVICE, "Name");
	if (value && g_variant_is_of_type(value, G_VARIANT_TYPE_STRING))
		text = hub_bluez_with_battery(device,
			g_variant_get_string(value, NULL));
	else
		text = NULL;
	if (value != NULL)
		g_variant_unref(value);
	return (text);
}

gchar	*hub_bluez_with_battery(GDBusObject *device, const gchar *name)
{
	GVariant	*value;
	gchar		*text;

	value = hub_bluez_prop(device, HUB_BLUEZ_BATTERY, "Percentage");
	if (value && g_variant_is_of_type(value, G_VARIANT_TYPE_BYTE))
		text = g_strdup_printf("%s · %u %%", name, g_variant_get_byte(value));
	else
		text = g_strdup_printf("%s · ...", name);
	if (value != NULL)
		g_variant_unref(value);
	return (text);
}
