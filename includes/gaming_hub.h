
#ifndef GAMING_HUB_H
# define GAMING_HUB_H

# include <gtk/gtk.h>
# include "libft.h"

// *** Functions *** //

// main.c
void	hub_activate(GtkApplication *self, gpointer user_data);
gboolean	hub_tick(gpointer user_data);

#endif
