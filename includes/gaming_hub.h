
#ifndef GAMING_HUB_H
# define GAMING_HUB_H

# include <gtk/gtk.h>
# include "libft.h"

// *** Struct *** //

typedef struct s_hub
{
	GtkWidget	*ram;
	GtkWidget	*cpu;
	guint64		prev_total;
	guint64		prev_idle;
}	t_hub;

// *** Functions *** //

// root/

// main.c
void		hub_activate(GtkApplication *self, gpointer user_data);
gboolean	hub_tick(gpointer user_data);

// sources/

// utils.c
void		hub_struct_init(t_hub *hub);

// ram.c
void		hub_read_ram(GtkLabel *label);
gboolean	hub_meminfo_value(const gchar *mem_start, gsize length_file,
		const char *key, guint64 *value);

// cpu.c
gboolean	hub_cpu_sample(guint64 *total, guint64 *idle);
gboolean	hub_getting_sample(guint64 *total, guint64 *idle,
		gchar **cpu_info, int *i);

#endif
