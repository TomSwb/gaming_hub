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

// === Enum === //

typedef enum e_timer_state
{
	TIMER_PLANNING,
	TIMER_PLAYING,
	TIMER_PAUSED,
	TIMER_BREAK,
	TIMER_ENDED
}	t_timer_state;

// === Struct === //

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

typedef struct s_settings
{
	gint	session;
	gint	wind_down;
	gint	break_min; // min = minutes
	gint	eye_min; // min = minutes
	gint	eye_sec;
}	t_settings;

typedef struct s_timer
{
	GtkWidget		*session;
	GtkWidget		*start;
	GtkWidget		*countdown;
	GtkWidget		*pause;
	GtkWidget		*stop;
	GtkWidget		*done;
	GtkWidget		*eye;
	GtkWidget		*break_spin;
	GtkWidget		*wind_spin;
	GtkWidget		*popups_check;
	GtkWidget		*sound_check;
	gint			planned;
	t_timer_state	state;
	gint64			started;
	gint64			paused_at;
	gint			next_break;
	gint			next_eye;
	gint			eye_until;
	gboolean		wind_warned;
	GtkMediaStream	*sound;
	gboolean		popups;
	gboolean		sound_on;
}	t_timer;

typedef struct s_hub
{
	t_computer		computer;
	t_settings		setting;
	t_timer			timer;
	GtkWidget		*stack;
	GApplication	*app;
}	t_hub;

#endif
