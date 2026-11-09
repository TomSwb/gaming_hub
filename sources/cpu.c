
#include "gaming_hub.h"

gboolean	hub_cpu_sample(guint64 *total, guint64 *idle)
{
	gchar	*cpu_info;
	gsize	length_file;
	int		i;

	*total = 0;
	*idle = 0;
	if (!g_file_get_contents("/proc/stat", &cpu_info, &length_file, NULL))
		return (FALSE);
	cpu_info += 4;
	i = 0;
	while (i < 8)
	{
		if (!hub_getting_sample(total, idle, cpu_info, &i))
			return (FALSE);
	}
	g_free(cpu_info);
	return (TRUE);
}

gboolean	hub_getting_sample(guint64 *total, guint64 *idle,
		gchar *cpu_info, int *i)
{
	guint64	value;
	gchar	*end;

	value = g_ascii_strtoull(cpu_info, &end, 10);
	if (end == cpu_info)
	{
		g_free(cpu_info);
		return (FALSE);
	}
	*total += value;
	if (*i == 3 || *i == 4)
		*idle += value;
	cpu_info = end;
	(*i)++;
}
