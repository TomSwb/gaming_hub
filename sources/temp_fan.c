
#include "gaming_hub.h"

void	hub_update_temp_fan(t_hub *hub)
{
	guint64	temp;
	guint64	rpm;

	hub_read_number(hub->temp_dir, "temp1_input", &temp, hub);
	hub_read_number(hub->fan_dir, "fan1_input", &rpm, hub);
	hub_set_temp_fan_text(hub, temp, rpm);
}

void	hub_set_temp_fan_text(t_hub *hub, guint64 temp, guint64 rpm)
{
	gchar	*text;

	text = g_strdup_printf("TEMP %.1f °C, FAN %d rpm", temp / 1000.0, (int)rpm);
	gtk_label_set_text(GTK_LABEL(hub->temp), text);
	g_free(text);
}

void	hub_temp_fan_done(gchar *contents, gchar *path, t_hub *hub,
		gboolean print_bool)
{
	g_free(contents);
	g_free(path);
	if (print_bool)
		gtk_label_set_text(GTK_LABEL(hub->temp), "TEMP -, FAN - rpm");
}
