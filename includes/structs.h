/* ************************************************************************** */
/*                                                                            */
/*                                            :::::::    :::     :::  :::::   */
/*   structs.h                                :+:   :+:  :+:+: :+:+:   :+:    */
/*                                            +#++:+#+   +#+ +:+ +#+   +#+    */
/*   By: TomSwb <github.com/TomSwb>           +#+   +#+  +#+     +#+   +#+    */
/*                                            ########   ###     ###  #####   */
/*   Created: 2026/11/10 23:07:28 by TomSwb                                   */
/*   Updated: 2026/11/10 23:07:28 by TomSwb                   bitminded.ch    */
/*                                                                            */
/* ************************************************************************** */

#ifndef STRUCTS_H
# define STRUCTS_H

// === Libraries === //

# include <gtk/gtk.h>

// === Struct === //

typedef struct s_settings
{
	gint	session;
	gint	wind_down;
	gint	break_min;
	gint	eye_min;
	gint	eye_sec;
}	t_settings;

typedef struct s_computer
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
}	t_computer;

typedef struct s_hub
{
	t_computer	*computer;
	t_settings	*setting;
}	t_hub;

#endif
