
#include "gaming_hub.h"

void	hub_read_ram(GtkLabel *label)
{
	gchar		*ram_info;
	gsize		length_file;
	guint64		memtotal;
	guint64		memavailable;
	gchar		*text;

	if (!g_file_get_contents("/proc/meminfo", &ram_info, &length_file, NULL))
		return ;
	if (!hub_meminfo_value(ram_info, length_file, "MemTotal:", &memtotal)
		|| !hub_meminfo_value(ram_info, length_file, "MemAvailable:",
			&memavailable))
	{
		g_free(ram_info);
		return ;
	}
	text = g_strdup_printf("RAM %.1f / %.1f GiB",
			(memtotal - memavailable) / (1024.0 * 1024.0),
			memtotal / (1024.0 * 1024.0));
	gtk_label_set_text(label, text);
	g_free(text);
	g_free(ram_info);
}

gboolean	hub_meminfo_value(const gchar *mem_start, gsize length_file,
		const char *key, guint64 *value)
{
	size_t	length_key;
	gchar	*mem_a_end;

	length_key = ft_strlen(key);
	mem_start = ft_strnstr(mem_start, key, length_file);
	if (!mem_start)
		return (FALSE);
	mem_start += length_key;
	*value = g_ascii_strtoull(mem_start, &mem_a_end, 10);
	if (mem_a_end == mem_start)
		return (FALSE);
	return (TRUE);
}
