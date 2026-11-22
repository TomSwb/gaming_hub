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
	GDBusProxy	*proxy;
	GError		*error;

	(void)source;
	hub = user_data;
	error = NULL;
	proxy = g_dbus_proxy_new_for_bus_finish(res, &error);
	if (!proxy)
	{
		g_printerr("power: %s\n", error->message);
		g_error_free(error);
		return ;
	}
	hub->power.proxy = proxy;
	hub_power_profiles(hub);
	hub_power_show(hub);
	g_signal_connect(hub->power.proxy, "g-properties-changed",
		G_CALLBACK(hub_power_changed), hub);
}

void	hub_power_show(t_hub *hub)
{
	GVariant	*profile;
	gchar		*text;

	text = NULL;
	profile = g_dbus_proxy_get_cached_property(hub->power.proxy,
			HUB_PPD_ACTIVE);
	if (profile && g_variant_is_of_type(profile, G_VARIANT_TYPE_STRING))
	{
		text = g_strdup_printf("Power profile: %s",
				g_variant_get_string(profile, NULL));
		gtk_label_set_text(GTK_LABEL(hub->power.profile), text);
	}
	else
		gtk_label_set_text(GTK_LABEL(hub->power.profile), "Power profile: ...");
	g_free(text);
	if (profile)
		g_variant_unref(profile);
}

void	hub_power_changed(GDBusProxy *proxy, GVariant *changed,
			GStrv invalidated, gpointer user_data)
{
	t_hub	*hub;

	(void)proxy;
	(void)changed;
	(void)invalidated;
	hub = user_data;
	hub_power_show(hub);
}
