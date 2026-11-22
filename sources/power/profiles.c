/* ************************************************************************** */
/*                                                                            */
/*                                            :::::::    :::     :::  :::::   */
/*   profiles.c                               :+:   :+:  :+:+: :+:+:   :+:    */
/*                                            +#++:+#+   +#+ +:+ +#+   +#+    */
/*   By: TomSwb <github.com/TomSwb>           +#+   +#+  +#+     +#+   +#+    */
/*                                            ########   ###     ###  #####   */
/*   Created: 2026/11/22 11:30:11 by TomSwb                                   */
/*   Updated: 2026/11/22 11:30:11 by TomSwb                   bitminded.ch    */
/*                                                                            */
/* ************************************************************************** */

#include "gaming_hub.h"

void	hub_power_profiles(t_hub *hub)
{
	GVariant		*profiles;
	GVariantIter	iter;
	GVariant		*dict;
	const gchar		*name;

	profiles = g_dbus_proxy_get_cached_property(hub->power.proxy,
			HUB_PPD_PROFILES);
	if (profiles == NULL
		|| !g_variant_is_of_type(profiles, G_VARIANT_TYPE("aa{sv}")))
	{
		if (profiles)
			g_variant_unref(profiles);
		g_printerr("power: no profiles\n");
		return ;
	}
	g_variant_iter_init(&iter, profiles);
	dict = g_variant_iter_next_value(&iter);
	while (dict != NULL)
	{
		if (g_variant_lookup(dict, "Profile", "&s", &name))
			hub_power_add_button(hub, name);
		g_variant_unref(dict);
		dict = g_variant_iter_next_value(&iter);
	}
	g_variant_unref(profiles);
}

void	hub_power_add_button(t_hub *hub, const gchar *name)
{
	GtkWidget	*button;
	GtkWidget	*leader;

	button = gtk_toggle_button_new_with_label(name);
	g_object_set_data_full(G_OBJECT(button), HUB_PPD_KEY,
		g_strdup(name), g_free);
	leader = gtk_widget_get_first_child(hub->power.pow_but_row);
	if (leader)
		gtk_toggle_button_set_group(GTK_TOGGLE_BUTTON(button),
			GTK_TOGGLE_BUTTON(leader));
	gtk_box_append(GTK_BOX(hub->power.pow_but_row), button);
	g_signal_connect(button, "toggled", G_CALLBACK(hub_power_toggled), hub);
}

void	hub_power_select(t_hub *hub, const gchar *active)
{
	GtkWidget	*child;
	const gchar	*name;

	child = gtk_widget_get_first_child(hub->power.pow_but_row);
	while (child != NULL)
	{
		name = g_object_get_data(G_OBJECT(child), HUB_PPD_KEY);
		g_signal_handlers_block_by_func(child,
			G_CALLBACK(hub_power_toggled), hub);
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(child),
			g_strcmp0(name, active) == 0);
		g_signal_handlers_unblock_by_func(child,
			G_CALLBACK(hub_power_toggled), hub);
		child = gtk_widget_get_next_sibling(child);
	}
}

void	hub_power_toggled(GtkToggleButton *self, gpointer user_data)
{
	const gchar	*name;
	t_hub		*hub;

	hub = user_data;
	if (gtk_toggle_button_get_active(self))
	{
		name = g_object_get_data(G_OBJECT(self), HUB_PPD_KEY);
		g_dbus_proxy_call(hub->power.proxy, HUB_PPD_METH_NAME,
			g_variant_new("(ssv)", HUB_PPD_IFACE, HUB_PPD_ACTIVE,
				g_variant_new_string(name)), G_DBUS_CALL_FLAGS_NONE,
			-1, NULL, hub_power_set_done, hub);
	}
}

void	hub_power_set_done(GObject *source, GAsyncResult *res,
			gpointer user_data)
{
	t_hub		*hub;
	GVariant	*profiles;
	GError		*error;

	hub = user_data;
	error = NULL;
	profiles = g_dbus_proxy_call_finish(G_DBUS_PROXY(source), res, &error);
	if (!profiles)
	{
		g_dbus_error_strip_remote_error(error);
		g_printerr("power: %s\n", error->message);
		g_error_free(error);
		hub_power_show(hub);
	}
	else
		g_variant_unref(profiles);
}
