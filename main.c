
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
	hub_free_hub(&hub);
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
	hub_setting_hub(hub);
	box = hub_build_box(hub);
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
	hub_update_temp_fan(hub);
	hub_update_gpu(hub);
	return (G_SOURCE_CONTINUE);
}
