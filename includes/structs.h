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
# include <pulse/pulseaudio.h>
# include <pulse/glib-mainloop.h>

// === Enums === //

typedef enum e_timer_state
{
	TIMER_PLANNING,
	TIMER_PLAYING,
	TIMER_PAUSED,
	TIMER_BREAK,
	TIMER_ENDED,
	TIMER_TRANSIT
}	t_timer_state;

typedef enum e_game_source
{
	GAME_STEAM,
	GAME_EMULATOR,
	GAME_MANUAL
}	t_game_source;

typedef enum e_game_prop
{
	GAME_PROP_NONE,
	GAME_PROP_NAME,
	GAME_PROP_SOURCE,
	GAME_PROP_PLATFORM,
	GAME_PROP_ID,
	GAME_PROP_PATH,
	GAME_PROP_COMMAND,
	GAME_PROP_COUNT
}	t_game_prop;

typedef enum e_kv_token
{
	KV_STRING,
	KV_OPEN,
	KV_CLOSE,
	KV_END,
	KV_ERROR
}	t_kv_token;

// === Structs === //

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
	gboolean	was_plugged;
	gboolean	low_warned;
}	t_computer;

typedef struct s_settings
{
	gint		session;
	gint		wind_down;
	gint		break_min; // min = minutes
	gint		eye_min; // min = minutes
	gint		eye_sec;
	gboolean	popups;
	gboolean	sound;
}	t_settings;

typedef struct s_timer
{
	GtkWidget		*session;
	GtkWidget		*start;
	GtkWidget		*countdown;
	GtkWidget		*transit_countdown;
	GtkWidget		*pause;
	GtkWidget		*stop;
	GtkWidget		*transit_end;
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

typedef struct s_headset
{
	GtkWidget			*volume;
	GtkWidget			*sld_vol;
	GtkWidget			*mute_vol;
	GtkWidget			*mic;
	GtkWidget			*sld_mic;
	GtkWidget			*mute_mic;
	GtkWidget			*headset_data;
	GtkWidget			*dev_row;
	pa_glib_mainloop	*adapter;
	pa_context			*context;
	uint32_t			sink_index;
	uint32_t			src_index;
	pa_cvolume			sink_vol;
	pa_cvolume			src_vol;
	gint64				moved_vol;
	gint64				moved_mic;
	GDBusObjectManager	*obj_man;
	gchar				*bt_add;
	gchar				*def_sink_name;
	gchar				*def_src_name;
	gchar				*want_add;
	gchar				*head_mic;
	gchar				*lapt_mic;
}	t_headset;

typedef struct s_power
{
	GtkWidget	*profile;
	GtkWidget	*pow_but_row;
	GDBusProxy	*proxy;
}	t_power;

typedef struct s_hub_game
{
	GObject			parent;
	t_game_source	source;
	gchar			*name;
	gchar			*platform;
	gchar			*id;
	gchar			*path;
	gchar			*command;
}	t_hub_game;

typedef struct s_game_info
{
	t_game_source	source;
	const gchar		*name;
	const gchar		*platform;
	const gchar		*id;
	const gchar		*path;
	const gchar		*command;
}	t_game_info;

typedef struct s_platform
{
	const gchar	*name;
	gchar		*folder;
	gchar		**extensions;
	gchar		*command;
}	t_platform;

typedef struct s_hub_game_class
{
	GObjectClass	parent_class;
}	t_hub_game_class;

typedef struct s_library
{
	GListStore	*games;
	t_hub_game	*picked;
	GtkWidget	*next;
	GtkWidget	*playing;
}	t_library;

typedef struct s_kv_cursor
{
	const char	*pos;
	gint		line;
}	t_kv_cursor;

typedef struct s_steam_app
{
	gchar	*id;
	gchar	*name;
	gchar	*dir;
	gchar	*owner;
}	t_steam_app;

typedef struct s_dump_name
{
	gchar	*title;
	gchar	*region;
	gchar	*id;
}	t_dump_name;

typedef struct s_hub
{
	t_computer		computer;
	t_settings		setting;
	t_timer			timer;
	t_headset		headset;
	t_power			power;
	t_library		library;
	GtkWidget		*stack;
	GtkWidget		*sections;
	GApplication	*app;
	guint			tick_id;
}	t_hub;

#endif
