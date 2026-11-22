/* ************************************************************************** */
/*                                                                            */
/*                                            :::::::    :::     :::  :::::   */
/*   main.c                                   :+:   :+:  :+:+: :+:+:   :+:    */
/*                                            +#++:+#+   +#+ +:+ +#+   +#+    */
/*   By: TomSwb <github.com/TomSwb>           +#+   +#+  +#+     +#+   +#+    */
/*                                            ########   ###     ###  #####   */
/*   Created: 2026/11/10 23:07:14 by TomSwb                                   */
/*   Updated: 2026/11/10 23:07:14 by TomSwb                   bitminded.ch    */
/*                                                                            */
/* ************************************************************************** */

#include "gaming_hub.h"

int	main(int ac, char **av)
{
	GtkApplication	*app;
	int				status;
	t_hub			hub;

	hub_struct_init(&hub);
	app = gtk_application_new("ch.bitminded.GamingHub",
			G_APPLICATION_HANDLES_COMMAND_LINE);
	g_signal_connect(app, "activate", G_CALLBACK(hub_activate), &hub);
	g_signal_connect(app, "command-line", G_CALLBACK(hub_command_line), &hub);
	g_application_add_main_option(G_APPLICATION(app), "end", 0,
		G_OPTION_FLAG_NONE, G_OPTION_ARG_NONE,
		"A game closed: the session goes between games", NULL);
	g_application_add_main_option(G_APPLICATION(app), "start", 0,
		G_OPTION_FLAG_NONE, G_OPTION_ARG_NONE,
		"A game started: the session carries on", NULL);
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
	window = gtk_application_get_active_window(self);
	if (window != NULL)
	{
		gtk_window_present(window);
		return ;
	}
	window = GTK_WINDOW(gtk_application_window_new(self));
	hub->app = G_APPLICATION(self);
	gtk_window_set_title(window, "Gaming Hub");
	gtk_window_set_default_size(window, 480, 720);
	hub_setting_hub(hub);
	box = hub_build_box(hub);
	gtk_window_set_child(window, box);
	g_timeout_add(1000, hub_tick, hub);
	hub_pulse_connect(hub);
	hub_power_connect(hub);
	hub_tick(hub);
	gtk_window_present(window);
}

gboolean	hub_tick(gpointer user_data)
{
	t_hub	*hub;

	hub = user_data;
	hub_read_ram(GTK_LABEL(hub->computer.ram));
	hub_update_cpu(hub);
	hub_update_temp_fan(hub);
	hub_update_gpu(hub);
	hub_update_battery(hub);
	hub_battery_unplug(hub);
	hub_update_timer(hub);
	return (G_SOURCE_CONTINUE);
}

int	hub_command_line(GApplication *self,
		GApplicationCommandLine *command_line, gpointer user_data)
{
	GVariantDict	*option;

	option = g_application_command_line_get_options_dict(command_line);
	if (g_variant_dict_contains(option, "end"))
		hub_game_closed(user_data);
	else
		g_application_activate(self);
	if (g_variant_dict_contains(option, "start"))
		hub_game_started(user_data);
	return (0);
}
