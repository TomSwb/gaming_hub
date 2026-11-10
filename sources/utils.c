
#include "gaming_hub.h"

void	hub_struct_init(t_hub *hub)
{
	hub->ram = NULL;
	hub->cpu = NULL;
	hub->temp = NULL;
	hub->gpu = NULL;
	hub->prev_total = 0;
	hub->prev_idle = 0;
	hub->temp_dir = NULL;
	hub->fan_dir = NULL;
	hub->gpu_dir = NULL;
	hub->gpu_max = 0;
}

void	hub_setting_hub(t_hub *hub)
{
	hub->ram = gtk_label_new("");
	hub->cpu = gtk_label_new("");
	hub->temp = gtk_label_new("");
	hub->gpu = gtk_label_new("");
	hub->temp_dir = hub_find_hwmon("coretemp");
	hub->fan_dir = hub_find_hwmon("thinkpad");
	hub->gpu_dir = hub_find_gpu();
	hub_read_number(hub->gpu_dir, "gt_RP0_freq_mhz",
			&hub->gpu_max);
}

GtkWidget	*hub_build_box(t_hub *hub)
{
	GtkWidget	*box;

	box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 12);
	gtk_box_append(GTK_BOX(box), hub->ram);
	gtk_box_append(GTK_BOX(box), hub->cpu);
	gtk_box_append(GTK_BOX(box), hub->temp);
	gtk_box_append(GTK_BOX(box), hub->gpu);
	return (box);
}

void	hub_free_hub(t_hub *hub)
{
	g_free(hub->temp_dir);
	g_free(hub->fan_dir);
	g_free(hub->gpu_dir);
}
