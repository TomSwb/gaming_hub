
#include "gaming_hub.h"

void	hub_update_temp_fan(t_hub *hub)
{
	guint64	temp;
	guint64	rpm;
	gchar	*text;

	temp = 0;
	rpm = 0;
	text = NULL;
	if (!hub_read_number(hub->temp_dir, "temp1_input", &temp)
		|| !hub_read_number(hub->fan_dir, "fan1_input", &rpm))
	{
		if (!hub_read_number(hub->fan_dir, "fan1_input", &rpm))
			text = g_strdup_printf("TEMP %.0f °C, FAN - rpm", temp / 1000.0);
		else if (!hub_read_number(hub->temp_dir, "temp1_input", &temp))
			text = g_strdup_printf("TEMP - °C, FAN %" G_GUINT64_FORMAT " rpm",
			rpm);
		else
			text = g_strdup_printf("TEMP - °C, FAN - rpm");
		gtk_label_set_text(GTK_LABEL(hub->temp), text);
		return ;
	}
	text = g_strdup_printf("TEMP %.0f °C, FAN %" G_GUINT64_FORMAT " rpm",
			temp / 1000.0, rpm);
	gtk_label_set_text(GTK_LABEL(hub->temp), text);
	g_free(text);
}
