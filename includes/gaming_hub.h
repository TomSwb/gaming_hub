
#ifndef GAMING_HUB_H
# define GAMING_HUB_H

// === Librairies === //

# include "libft.h"
# include "structs.h"
# include "computer.h"

// === Functions === //

// root/

// sources/

// main.c
void		hub_activate(GtkApplication *self, gpointer user_data);
gboolean	hub_tick(gpointer user_data);

// utils.c
void		hub_struct_init(t_hub *hub);
void		hub_setting_hub(t_hub *hub);
GtkWidget	*hub_build_box(t_hub *hub);
void		hub_free_hub(t_hub *hub);


#endif
