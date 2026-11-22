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
			g_print("power: has %s\n", name);
		g_variant_unref(dict);
		dict = g_variant_iter_next_value(&iter);
	}
	g_variant_unref(profiles);
}
