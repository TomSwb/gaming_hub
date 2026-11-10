
#include "gaming_hub.h"

void	hub_update_temp_fan(t_hub *hub)
{
	guint64		temp;
	guint64		rpm;
	gchar		*text;

	temp = 0;
	rpm = 0;
	text = NULL;
	if (!hub_temp_fan_error_path(hub, &temp, &rpm))
		return ;
	text = g_strdup_printf("TEMP %.0f °C, FAN %" G_GUINT64_FORMAT " rpm",
			temp / 1000.0, rpm);
	gtk_label_set_text(GTK_LABEL(hub->temp), text);
	g_free(text);
}

gboolean	hub_temp_fan_error_path(t_hub *hub, guint64 *temp, guint64 *rpm)
{
	gboolean	temp_ok;
	gboolean	fan_ok;
	gchar		*text;

	temp_ok = hub_read_number(hub->temp_dir, "temp1_input", temp);
	fan_ok = hub_read_number(hub->fan_dir, "fan1_input", rpm);
	if (!temp_ok || !fan_ok)
	{
		if (!temp_ok && !fan_ok)
			text = g_strdup_printf("TEMP ... °C, FAN ... rpm");
		else if (!temp_ok)
			text = g_strdup_printf("TEMP ... °C, FAN %" G_GUINT64_FORMAT
			" rpm", *rpm);
		else
			text = g_strdup_printf("TEMP %.0f °C, FAN ... rpm", *temp / 1000.0);
		gtk_label_set_text(GTK_LABEL(hub->temp), text);
		g_free(text);
		return (FALSE);
	}
	return (TRUE);
}
