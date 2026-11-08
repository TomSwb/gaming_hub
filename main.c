
#include "gaming_hub.h"

int	main(int ac, char **av)
{
	GtkApplication	*app;
	int				status;

	app = gtk_application_new("ch.bitminded.GamingHub",
			G_APPLICATION_DEFAULT_FLAGS);
	g_signal_connect(app, "activate", G_CALLBACK(hub_activate), NULL);
	status = g_application_run(G_APPLICATION(app), ac, av);
}

void	hub_activate(GtkApplication *self, gpointer user_data)
{
	(void)user_data;
	(void)self;
	ft_printf("Hello World\n");
}
