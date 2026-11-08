
#include "gaming_hub.h"

int	main(int ac, char **av)
{
	GtkApplication	*app;
	int				status;

	app = gtk_application_new("ch.bitminded.GamingHub",
			G_APPLICATION_DEFAULT_FLAGS);
	g_signal_connect(app, "activate", G_CALLBACK(hub_activate), NULL);
	status = g_application_run(G_APPLICATION(app), ac, av);
	g_object_unref(app);
	return (status);
}

void	hub_activate(GtkApplication *self, gpointer user_data)
{
	GtkWindow	*window;
	GtkWidget	*label;

	(void)user_data;
	window = GTK_WINDOW(gtk_application_window_new(self));
	gtk_window_set_title(window, "Gaming Hub");
	gtk_window_set_default_size(window, 480, 720);
	label = gtk_label_new("Hello World!");
	gtk_window_set_child(window, label);
	g_timeout_add_seconds(1, hub_tick, label);
	gtk_window_present(window);
}

gboolean	hub_tick(gpointer user_data)
{
	GtkLabel	*label;

	label = GTK_LABEL(user_data);
	gtk_label_set_text(label, "It's time to game!");
	return(G_SOURCE_CONTINUE);
}
