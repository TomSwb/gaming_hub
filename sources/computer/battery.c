/* ************************************************************************** */
/*                                                                            */
/*                                            :::::::    :::     :::  :::::   */
/*   battery.c                                :+:   :+:  :+:+: :+:+:   :+:    */
/*                                            +#++:+#+   +#+ +:+ +#+   +#+    */
/*   By: TomSwb <github.com/TomSwb>           +#+   +#+  +#+     +#+   +#+    */
/*                                            ########   ###     ###  #####   */
/*   Created: 2026/11/10 23:07:10 by TomSwb                                   */
/*   Updated: 2026/11/10 23:07:10 by TomSwb                   bitminded.ch    */
/*                                                                            */
/* ************************************************************************** */

#include "gaming_hub.h"

void	hub_update_battery(t_hub *hub)
{
	guint64		capacity;
	gchar		*charging;
	gchar		*text;

	capacity = 0;
	text = NULL;
	charging = hub_read_text(hub->computer.bat_dir, "status");
	if (!charging)
		charging = hub_battery_set_charging(hub);
	if (!hub_battery_error_path(hub, &capacity, charging))
	{
		g_free(charging);
		return ;
	}
	text = g_strdup_printf("BAT %" G_GUINT64_FORMAT " %%, %s",
			capacity, charging);
	gtk_label_set_text(GTK_LABEL(hub->computer.bat), text);
	g_free(charging);
	g_free(text);
}

gboolean	hub_battery_error_path(t_hub *hub, guint64 *capacity,
			gchar *charging)
{
	gboolean	capacity_ok;
	gchar		*text;

	capacity_ok = hub_read_number(hub->computer.bat_dir,
			"capacity", capacity);
	if (!capacity_ok)
	{
		text = g_strdup_printf("BAT ... %%, %s", charging);
		gtk_label_set_text(GTK_LABEL(hub->computer.bat), text);
		g_free(text);
		return (FALSE);
	}
	return (TRUE);
}

gchar	*hub_battery_set_charging(t_hub *hub)
{
	guint64		plugged;
	gboolean	plugged_ok;
	gchar		*charging;

	plugged_ok = hub_read_number(hub->computer.mains_dir, "online", &plugged);
	if (!plugged_ok)
		charging = g_strdup("...");
	else if (!plugged)
		charging = g_strdup("on battery");
	else
		charging = g_strdup("plugged in");
	return (charging);
}

void	hub_battery_unplug(t_hub *hub)
{
	guint64		plugged;
	gboolean	plugged_ok;
	gboolean	plugged_now;

	plugged_ok = hub_read_number(hub->computer.mains_dir, "online", &plugged);
	if (!plugged_ok)
		return ;
	plugged_now = plugged != 0;
	if (!plugged_now && hub->computer.was_plugged && hub_timer_running(hub))
		hub_alert(hub, "unplugged", "Charger unplugged", "Running on battery");
	hub->computer.was_plugged = plugged_now;
}

void	hub_battery_low(t_hub *hub)
{
	guint64	value;
	gchar	*text;

	if (!hub_read_number(hub->computer.bat_dir, "capacity", &value))
		return ;
	if (hub->computer.was_plugged)
	{
		hub->computer.low_warned = FALSE;
		return ;
	}
	if (!hub->computer.low_warned && value <= HUB_BAT_LOW
		&& hub_timer_running(hub))
	{
		text = g_strdup_printf("Battery at %" G_GUINT64_FORMAT " %%", value);
		hub_alert(hub, "battery_low", "Battery low", text);
		g_free(text);
		hub->computer.low_warned = TRUE;
	}
}
