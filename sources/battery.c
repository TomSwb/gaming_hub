
#include "gaming_hub.h"

void	hub_update_battery(t_hub *hub)
{
	guint64		capacity;
	gchar		*charging;
	gchar		*text;

	capacity = 0;
	text = NULL;
	charging = hub_read_text(hub->bat_dir, "status");
	if (!charging)
		charging = hub_battery_set_charging(hub);
	if (!hub_battery_error_path(hub, &capacity, charging))
	{
		g_free(charging);
		return ;
	}
	text = g_strdup_printf("BAT %" G_GUINT64_FORMAT " %%, %s",
			capacity, charging);
	gtk_label_set_text(GTK_LABEL(hub->bat), text);
	g_free(charging);
	g_free(text);
}

gboolean	hub_battery_error_path(t_hub *hub, guint64 *capacity,
			gchar *charging)
{
	gboolean	capacity_ok;
	gchar		*text;

	capacity_ok = hub_read_number(hub->bat_dir, "capacity", capacity);
	if (!capacity_ok)
	{
		text = g_strdup_printf("BAT ... %%, %s", charging);
		gtk_label_set_text(GTK_LABEL(hub->bat), text);
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

	plugged_ok = hub_read_number(hub->mains_dir, "online", &plugged);
	if (!plugged_ok)
		charging = g_strdup("...");
	else if (!plugged)
		charging = g_strdup("on battery");
	else
		charging = g_strdup("plugged in");
	return (charging);
}
