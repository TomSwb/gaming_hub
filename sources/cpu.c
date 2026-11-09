
#include "gaming_hub.h"

gboolean	hub_cpu_sample(guint64 *total, guint64 *idle)
{
	gchar	*cpu_info;
	gsize	length_file;
	gchar	*end;
	int		i;
	guint64	sum;

	if (!g_file_get_contents("/proc/meminfo", &cpu_info, &length_file, NULL))
		return (FALSE);
	cpu_info += 4;
	i = 0;
	while (i < 8)
	{
		sum = 0;
		sum += g_ascii_strtoll(cpu_info, &end, 10);
		if (end == cpu_info)
			return (FALSE);
		*total += sum;
		if (i == 3 || i == 4)
			*idle += sum;
		cpu_info = end;
		i++;
	}
	return (TRUE);
}
