
#include "gaming_hub.h"

void	hub_update_battery(t_hub *hub)
{
	guint64		capacity;
	guint64		plugged;
	gchar		*charging;
	gchar		*text;

	capacity = 0;
	plugged = 0;
	text = NULL;
	charging = hub_read_text(hub->bat_dir, "status");
	if (!charging)
		charging = "...";
	if (!hub_battery_error_path(hub, &capacity, &plugged, charging))
		return ;
	text = g_strdup_printf("BAT %" G_GUINT64_FORMAT " %%, %s",
			capacity, charging);
	gtk_label_set_text(GTK_LABEL(hub->bat), text);
	g_free(charging);
	g_free(text);
}

gboolean	hub_battery_error_path(t_hub *hub, guint64 *capacity,
			guint64 *plugged, gchar *charging)
{
	gboolean	capacity_ok;
	gboolean	plugged_ok;
	gchar		*text;

	capacity_ok = hub_read_number(hub->bat_dir, "capacity", capacity);
	plugged_ok = hub_read_number(hub->mains_dir, "online", plugged);
	if (!capacity_ok || !plugged_ok)
	{
		if (!capacity_ok)
			text = g_strdup_printf("BAT - %%, %s", charging);
		else
			text = g_strdup_printf("BAT %" G_GUINT64_FORMAT " %%, %s", *capacity, charging);
		gtk_label_set_text(GTK_LABEL(hub->bat), text);
		g_free(charging);
		g_free(text);
		return (FALSE);
	}
	return (TRUE);
}
