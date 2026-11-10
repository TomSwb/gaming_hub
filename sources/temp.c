
#include "gaming_hub.h"

void	hub_update_temp(t_hub *hub)
{
	gchar	*path;
	gchar	*contents;
	gchar	*end;
	guint64	value;

	if (!hub->temp_dir)
	{
		hub_temp_free(NULL, NULL, hub, 1);
		return ;
	}
	path = g_build_filename(hub->temp_dir, "temp1_input", NULL);
	if (!g_file_get_contents(path, &contents, NULL, NULL))
	{
		hub_temp_free(NULL, path, hub, 1);
		return ;
	}
	value = g_ascii_strtoull(contents, &end, 10);
	if (end == contents)
	{
		hub_temp_free(contents, path, hub, 1);
		return ;
	}
	hub_temp_free(contents, path, hub, 0);
	hub_set_temp_text(hub, value);
}

void	hub_set_temp_text(t_hub *hub, guint64 temp)
{
	gchar	*text;

	text = g_strdup_printf("TEMP %.0f °C", temp / 1000.0);
	gtk_label_set_text(GTK_LABEL(hub->temp), text);
	g_free(text);
}

void	hub_temp_free(gchar *contents, gchar *path, t_hub *hub, int print_bool)
{
	g_free(contents);
	g_free(path);
	if (print_bool)
		gtk_label_set_text(GTK_LABEL(hub->temp), "TEMP -");
}
