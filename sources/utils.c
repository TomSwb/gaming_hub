/* ************************************************************************** */
/*                                                                            */
/*                                            :::::::    :::     :::  :::::   */
/*   utils.c                                  :+:   :+:  :+:+: :+:+:   :+:    */
/*                                            +#++:+#+   +#+ +:+ +#+   +#+    */
/*   By: TomSwb <github.com/TomSwb>           +#+   +#+  +#+     +#+   +#+    */
/*                                            ########   ###     ###  #####   */
/*   Created: 2026/11/10 23:07:18 by TomSwb                                   */
/*   Updated: 2026/11/10 23:07:18 by TomSwb                   bitminded.ch    */
/*                                                                            */
/* ************************************************************************** */

#include "gaming_hub.h"

void	hub_struct_init(t_hub *hub)
{
	hub->computer.ram = NULL;
	hub->computer.cpu = NULL;
	hub->computer.temp = NULL;
	hub->computer.gpu = NULL;
	hub->computer.bat = NULL;
	hub->computer.prev_total = 0;
	hub->computer.prev_idle = 0;
	hub->computer.temp_dir = NULL;
	hub->computer.fan_dir = NULL;
	hub->computer.gpu_dir = NULL;
	hub->computer.gpu_max = 0;
	hub->computer.bat_dir = NULL;
	hub->computer.mains_dir = NULL;
	hub->setting.session = HUB_DEF_SESSION;
	hub->setting.wind_down = HUB_DEF_WIND_DOWN;
	hub->setting.break_min = HUB_DEF_BREAK_MIN;
	hub->setting.eye_min = HUB_DEF_EYE_CUE_MIN;
	hub->setting.eye_sec = HUB_DEF_EYE_CUE_LENGTH;
	hub->timer.session = NULL;
}

void	hub_setting_hub(t_hub *hub)
{
	hub_load_timer_settings(hub);
	hub->computer.ram = gtk_label_new("");
	hub->computer.cpu = gtk_label_new("");
	hub->computer.temp = gtk_label_new("");
	hub->computer.gpu = gtk_label_new("");
	hub->computer.bat = gtk_label_new("");
	hub->computer.temp_dir = hub_find_dir("/sys/class/hwmon",
			"name", "coretemp");
	hub->computer.fan_dir = hub_find_dir("/sys/class/hwmon",
			"name", "thinkpad");
	hub->computer.gpu_dir = hub_find_gpu();
	hub_read_number(hub->computer.gpu_dir, "gt_RP0_freq_mhz",
		&hub->computer.gpu_max);
	hub->computer.bat_dir = hub_find_dir("/sys/class/power_supply",
			"type", "Battery");
	hub->computer.mains_dir = hub_find_dir("/sys/class/power_supply",
			"type", "Mains");
	hub->timer.session = gtk_spin_button_new_with_range(HUB_SESSION_LOWEST,
			HUB_SESSION_HIGHEST, HUB_SESSION_STEP);
	gtk_spin_button_set_value(GTK_SPIN_BUTTON(hub->timer.session),
			hub->setting.session);
}

GtkWidget	*hub_build_box(t_hub *hub)
{
	GtkWidget	*box;

	box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 12);
	gtk_box_append(GTK_BOX(box), hub->computer.ram);
	gtk_box_append(GTK_BOX(box), hub->computer.cpu);
	gtk_box_append(GTK_BOX(box), hub->computer.temp);
	gtk_box_append(GTK_BOX(box), hub->computer.gpu);
	gtk_box_append(GTK_BOX(box), hub->computer.bat);
	gtk_box_append(GTK_BOX(box), hub->timer.session);
	return (box);
}

void	hub_free_hub(t_hub *hub)
{
	g_free(hub->computer.temp_dir);
	g_free(hub->computer.fan_dir);
	g_free(hub->computer.gpu_dir);
	g_free(hub->computer.bat_dir);
	g_free(hub->computer.mains_dir);
}
