/* ************************************************************************** */
/*                                                                            */
/*                                            :::::::    :::     :::  :::::   */
/*   gaming_hub.h                             :+:   :+:  :+:+: :+:+:   :+:    */
/*                                            +#++:+#+   +#+ +:+ +#+   +#+    */
/*   By: TomSwb <github.com/TomSwb>           +#+   +#+  +#+     +#+   +#+    */
/*                                            ########   ###     ###  #####   */
/*   Created: 2026/11/10 23:07:25 by TomSwb                                   */
/*   Updated: 2026/11/10 23:07:25 by TomSwb                   bitminded.ch    */
/*                                                                            */
/* ************************************************************************** */

#ifndef GAMING_HUB_H
# define GAMING_HUB_H

// === Librairies === //

# include "libft.h"
# include "structs.h"
# include "defaults.h"
# include "computer.h"
# include "timer.h"
# include "game.h"
# include "headset.h"
# include "power.h"

// === Functions === //

// sources/

// main.c
void		hub_activate(GtkApplication *self, gpointer user_data);
gboolean	hub_tick(gpointer user_data);
int			hub_command_line(GApplication *self,
				GApplicationCommandLine *command_line, gpointer user_data);

// utils.c
void		hub_setting_hub(t_hub *hub);
void		hub_free_hub(t_hub *hub);

// inits.c
void		hub_struct_init(t_hub *hub);
void		hub_computer_init(t_hub *hub);
void		hub_setting_init(t_hub *hub);
void		hub_timer_init(t_hub *hub);
void		hub_headset_init(t_hub *hub);

// inits_2.c
void		hub_power_init(t_hub *hub);

// main_build.c
GtkWidget	*hub_build_box(t_hub *hub);
GtkWidget	*hub_image_with_label(const gchar *icon, const gchar *text);
GtkWidget	*hub_labelled(const gchar *text, GtkWidget *widget);

#endif
