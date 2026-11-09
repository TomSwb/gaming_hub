
#include "gaming_hub.h"

gboolean	hub_cpu_sample(guint64 *total, guint64 *idle)
{
	const char	*p;
	char		*end;
	int			i;
	guint64		sum;

	p = "/proc/stat";
	p += 4;
	i = 0;
	while (i < 9)
	{
		sum += g_ascii_strtoll(p, &end, 10);
		*total += sum;
		if (i == 4 || i == 5)
			*idle += sum;
		if (end == p)
			return (FALSE);
		p = end;
		sum = 0;
		i++;
	}
	return (TRUE);
}
