
#include "gaming_hub.h"

void	hub_update_cpu(t_hub *hub)
{
	guint64	total;
	guint64	idle;
	guint64	dtotal;
	guint64	didle;
	double	percent;

	if (!hub_cpu_sample(&total, &idle))
		return ;
	if (hub->prev_total == 0)
	{
		hub->prev_total = total;
		hub->prev_idle = idle;
		gtk_label_set_text(GTK_LABEL(hub->cpu), "CPU -");
		return ;
	}
	dtotal = total - hub->prev_total;
	if (dtotal == 0)
		return ;
	didle = idle - hub->prev_idle;
	percent = 100.0 * (dtotal - didle) / dtotal;
	hub->prev_total = total;
	hub->prev_idle = idle;
	hub_set_cpu_text(hub, percent);
}

gboolean	hub_cpu_sample(guint64 *total, guint64 *idle)
{
	gchar	*p;
	gchar	*cpu_info;
	gsize	length_file;
	int		i;

	*total = 0;
	*idle = 0;
	if (!g_file_get_contents("/proc/stat", &p, &length_file, NULL))
		return (FALSE);
	cpu_info = p + 4;
	i = 0;
	while (i < 8)
	{
		if (!hub_getting_sample(total, idle, &cpu_info, &i))
		{
			g_free(p);
			return (FALSE);
		}
	}
	g_free(p);
	return (TRUE);
}

gboolean	hub_getting_sample(guint64 *total, guint64 *idle,
		gchar **cpu_info, int *i)
{
	guint64	value;
	gchar	*end;

	value = g_ascii_strtoull(*cpu_info, &end, 10);
	if (end == *cpu_info)
		return (FALSE);
	*total += value;
	if (*i == 3 || *i == 4)
		*idle += value;
	*cpu_info = end;
	(*i)++;
	return (TRUE);
}

void	hub_set_cpu_text(t_hub *hub, double percent)
{
	gchar	*text;

	text = g_strdup_printf("CPU %.1f %%", percent);
	gtk_label_set_text(GTK_LABEL(hub->cpu), text);
	g_free(text);
}
