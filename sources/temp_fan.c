
#include "gaming_hub.h"

void	hub_update_temp_fan(t_hub *hub)
{
	guint64	temp;
	guint64	rpm;

	temp = 0;
	rpm = 0;
	if (!hub_read_number(hub->temp_dir, "temp1_input", &temp)
		|| !hub_read_number(hub->fan_dir, "fan1_input", &rpm))
	{
		gtk_label_set_text(GTK_LABEL(hub->temp), "TEMP -, FAN - rpm");
		return ;
	}
	hub_set_temp_fan_text(hub, temp, rpm);
}

void	hub_set_temp_fan_text(t_hub *hub, guint64 temp, guint64 rpm)
{
	gchar	*text;

	text = g_strdup_printf("TEMP %.1f °C, FAN %" G_GUINT64_FORMAT " rpm",
			temp / 1000.0, rpm);
	gtk_label_set_text(GTK_LABEL(hub->temp), text);
	g_free(text);
}
