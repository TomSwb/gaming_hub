/* ************************************************************************** */
/*                                                                            */
/*                                            :::::::    :::     :::  :::::   */
/*   library.h                                :+:   :+:  :+:+: :+:+:   :+:    */
/*                                            +#++:+#+   +#+ +:+ +#+   +#+    */
/*   By: TomSwb <github.com/TomSwb>           +#+   +#+  +#+     +#+   +#+    */
/*                                            ########   ###     ###  #####   */
/*   Created: 2026/11/23 10:06:23 by TomSwb                                   */
/*   Updated: 2026/11/23 10:06:23 by TomSwb                   bitminded.ch    */
/*                                                                            */
/* ************************************************************************** */

#ifndef LIBRARY_H
# define LIBRARY_H

// === Library === //

# include "structs.h"

// === Functions === //

// sources/library/

// hub_game.c
GType		hub_game_get_type(void);
void		hub_game_class_init(gpointer klass, gpointer data);
void		hub_game_init(GTypeInstance *instance, gpointer klass);
void		hub_game_finalize(GObject *object);
t_hub_game	*hub_game_new(const t_game_info *info);

// hub_game_props.c
void		hub_game_install_props(GObjectClass *object_class);
void		hub_game_set_property(GObject *object, guint prop_id,
				const GValue *value, GParamSpec *pspec);
void		hub_game_get_property(GObject *object, guint prop_id,
				GValue *value, GParamSpec *pspec);
t_hub_game	*hub_game_cast(gpointer object);

// library_store.c
void		hub_library_add(GListStore *store, const t_game_info *info);

// library_rows.c
void		hub_row_setup(GtkSignalListItemFactory *self, GObject *object,
				gpointer user_data);
void		hub_row_bind(GtkSignalListItemFactory *self, GObject *object,
				gpointer user_data);

// library_build.c
GtkWidget	*hub_build_library(t_hub *hub);

// keyvalues.c
gboolean	hub_kv_enter(t_kv_cursor *cursor, const gchar *key);
t_kv_token	hub_kv_next(t_kv_cursor *cursor, gchar **text);
void		hub_kv_skip_space(t_kv_cursor *cursor);
t_kv_token	hub_kv_read_string(t_kv_cursor *cursor, gchar **text);
gboolean	hub_kv_skip_block(t_kv_cursor *cursor);

// keyvalues_2.c
gchar		*hub_kv_load(const gchar *path, t_kv_cursor *cursor);
t_kv_token	hub_kv_pair(t_kv_cursor *cursor, gchar **name,
				gchar **value);
void		hub_kv_take(gchar **slot, gchar **value);

// steam.c
void		hub_steam_scan(GListStore *store);
gboolean	hub_steam_folders(GListStore *store, t_kv_cursor *cursor);
void		hub_steam_library(GListStore *store, const gchar *lib);
gchar		*hub_steam_lib_path(t_kv_cursor *cursor);
void		hub_steam_app_free(t_steam_app *app);

// steam_app.c
void		hub_steam_app(GListStore *store, const gchar *folder,
				const gchar *name);
void		hub_steam_app_keep(t_steam_app *app, const gchar *key,
				gchar **value);
gboolean	hub_steam_app_read(t_kv_cursor *cursor, t_steam_app *app);
gboolean	hub_steam_app_skip(const t_steam_app *app);
void		hub_steam_app_add(GListStore *store, t_steam_app *app,
				const gchar *folder);

// sources/library/platforms.c
void	hub_platforms_scan(GListStore *store);

#endif
