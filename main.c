
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
	hub_read_ram();
	return (G_SOURCE_CONTINUE);
}

void	hub_read_ram(void)
{
	gchar		*ram_info;
	gsize		length_file;
	guint64		memtotal;
	guint64		memavailable;

	if (!g_file_get_contents("/proc/meminfo", &ram_info, &length_file, NULL))
		return ;
	if (!hub_meminfo_value(ram_info, length_file, "MemTotal:", &memtotal)
		|| !hub_meminfo_value(ram_info, length_file, "MemAvailable:",
			&memavailable))
	{
		g_free(ram_info);
		return ;
	}
	g_print("%lu|\n", memtotal - memavailable);
	g_free(ram_info);
}

gboolean	hub_meminfo_value(const gchar *mem_start, gsize length_file,
		const char *key, guint64 *value)
{
	size_t	length_key;
	gchar	*mem_a_end;

	length_key = ft_strlen(key);
	mem_start = ft_strnstr(mem_start, key, length_file);
	if (!mem_start)
		return (FALSE);
	mem_start += length_key;
	*value = g_ascii_strtoull(mem_start, &mem_a_end, 10);
	if (mem_a_end == mem_start)
		return (FALSE);
	return (TRUE);
}
