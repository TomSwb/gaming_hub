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
}

void	hub_setting_hub(t_hub *hub)
{
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
