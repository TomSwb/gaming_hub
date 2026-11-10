/* ************************************************************************** */
/*                                                                            */
/*                                            :::::::    :::     :::  :::::   */
/*   computer.h                               :+:   :+:  :+:+: :+:+:   :+:    */
/*                                            +#++:+#+   +#+ +:+ +#+   +#+    */
/*   By: TomSwb <github.com/TomSwb>           +#+   +#+  +#+     +#+   +#+    */
/*                                            ########   ###     ###  #####   */
/*   Created: 2026/11/10 23:07:22 by TomSwb                                   */
/*   Updated: 2026/11/10 23:07:22 by TomSwb                   bitminded.ch    */
/*                                                                            */
/* ************************************************************************** */

#ifndef COMPUTER_H
# define COMPUTER_H

// === Library === //

# include "structs.h"

// === Functions === //

// sources/computer/

// ram.c
void		hub_read_ram(GtkLabel *label);
gboolean	hub_meminfo_value(const gchar *mem_start, gsize length_file,
				const char *key, guint64 *value);
gchar		*hub_ram_set_text(guint64 memtotal, guint64 memavailable);

// cpu.c
void		hub_update_cpu(t_hub *hub);
gboolean	hub_cpu_sample(guint64 *total, guint64 *idle);
gboolean	hub_getting_sample(guint64 *total, guint64 *idle,
				gchar **cpu_info, int *i);
void		hub_set_cpu_text(t_hub *hub, double percent);
gboolean	hub_cpu_error_path(t_hub *hub, guint64 *total, guint64 *idle);

// temp_fan.c
void		hub_update_temp_fan(t_hub *hub);
gboolean	hub_temp_fan_error_path(t_hub *hub, guint64 *temp, guint64 *rpm);

// gpu.c
void		hub_update_gpu(t_hub *hub);

// battery.c
void		hub_update_battery(t_hub *hub);
gboolean	hub_battery_error_path(t_hub *hub, guint64 *capacity,
				gchar *charging);
gchar		*hub_battery_set_charging(t_hub *hub);

// sysfs.c
gchar		*hub_find_dir(const char *class_dir, const char *file,
				const char *value);
gchar		*hub_find_dir_loop(const char *class_dir, const char *file,
				const char *value, const gchar *entry);
gchar		*hub_find_gpu(void);
gboolean	hub_read_number(const gchar *dir, const gchar *file,
				guint64 *value);
gchar		*hub_read_text(const gchar *dir, const gchar *file);

#endif
