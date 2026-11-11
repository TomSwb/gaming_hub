/* ************************************************************************** */
/*                                                                            */
/*                                            :::::::    :::     :::  :::::   */
/*   timer.h                                  :+:   :+:  :+:+: :+:+:   :+:    */
/*                                            +#++:+#+   +#+ +:+ +#+   +#+    */
/*   By: TomSwb <github.com/TomSwb>           +#+   +#+  +#+     +#+   +#+    */
/*                                            ########   ###     ###  #####   */
/*   Created: 2026/11/11 09:23:40 by TomSwb                                   */
/*   Updated: 2026/11/11 09:23:40 by TomSwb                   bitminded.ch    */
/*                                                                            */
/* ************************************************************************** */

#ifndef TIMER_H
# define TIMER_H

// === Library === //

# include "structs.h"

// === Functions === //

// sources/timer/

// settings.c
void		hub_load_settings(t_hub *hub);
gboolean	hub_key_file_error(GKeyFile *key_file, gchar *path, GError *error);
gboolean	hub_read_settings(GKeyFile *key_file, const gchar *group,
				const gchar *key, gint *value);

#endif
