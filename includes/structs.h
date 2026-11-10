
#ifndef STRUCTS_H
# define STRUCTS_H

// === Libraries === //

# include <gtk/gtk.h>

// === Struct === //

typedef struct s_hub
{
	GtkWidget	*ram;
	GtkWidget	*cpu;
	GtkWidget	*temp;
	GtkWidget	*gpu;
	GtkWidget	*bat;
	guint64		prev_total;
	guint64		prev_idle;
	gchar		*temp_dir;
	gchar		*fan_dir;
	gchar		*gpu_dir;
	guint64		gpu_max;
	gchar		*bat_dir;
	gchar		*mains_dir;
}	t_hub;

#endif
