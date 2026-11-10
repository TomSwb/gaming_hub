
#include "gaming_hub.h"

int	main(int ac, char **av)
{
	GtkApplication	*app;
	int				status;
	t_hub			hub;

	hub_struct_init(&hub);
	app = gtk_application_new("ch.bitminded.GamingHub",
			G_APPLICATION_DEFAULT_FLAGS);
	g_signal_connect(app, "activate", G_CALLBACK(hub_activate), &hub);
	status = g_application_run(G_APPLICATION(app), ac, av);
	g_object_unref(app);
	g_free(hub.temp_dir);
	g_free(hub.fan_dir);
	return (status);
}

void	hub_activate(GtkApplication *self, gpointer user_data)
{
	GtkWindow	*window;
	GtkWidget	*box;
	t_hub		*hub;

	hub = user_data;
	window = GTK_WINDOW(gtk_application_window_new(self));
	gtk_window_set_title(window, "Gaming Hub");
	gtk_window_set_default_size(window, 480, 720);
	hub->ram = gtk_label_new("");
	hub->cpu = gtk_label_new("");
	hub->temp_dir = hub_find_hwmon("coretemp");
	hub->fan_dir = hub_find_hwmon("thinkpad");
	box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 12);
	gtk_box_append(GTK_BOX(box), hub->ram);
	gtk_box_append(GTK_BOX(box), hub->cpu);
	gtk_window_set_child(window, box);
	g_timeout_add_seconds(1, hub_tick, hub);
	hub_tick(hub);
	gtk_window_present(window);
}

gboolean	hub_tick(gpointer user_data)
{
	t_hub	*hub;

	hub = user_data;
	hub_read_ram(GTK_LABEL(hub->ram));
	hub_update_cpu(hub);
	return (G_SOURCE_CONTINUE);
}
