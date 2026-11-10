/* ************************************************************************** */
/*                                                                            */
/*                                            :::::::    :::     :::  :::::   */
/*   ram.c                                    :+:   :+:  :+:+: :+:+:   :+:    */
/*                                            +#++:+#+   +#+ +:+ +#+   +#+    */
/*   By: TomSwb <github.com/TomSwb>           +#+   +#+  +#+     +#+   +#+    */
/*                                            ########   ###     ###  #####   */
/*   Created: 2026/11/10 23:06:59 by TomSwb                                   */
/*   Updated: 2026/11/10 23:06:59 by TomSwb                   bitminded.ch    */
/*                                                                            */
/* ************************************************************************** */

#include "gaming_hub.h"

void	hub_read_ram(GtkLabel *label)
{
	gchar		*ram_info;
	guint64		memtotal;
	guint64		memavailable;
	gsize		length_file;
	gchar		*text;

	text = NULL;
	memtotal = 0;
	memavailable = 0;
	if (!g_file_get_contents("/proc/meminfo", &ram_info, &length_file, NULL))
	{
		gtk_label_set_text(label, "RAM ... GiB");
		return ;
	}
	if (!hub_meminfo_value(ram_info, length_file, "MemTotal:", &memtotal)
		|| !hub_meminfo_value(ram_info, length_file,
			"MemAvailable:", &memavailable))
		text = g_strdup("RAM ... GiB");
	else
		text = hub_ram_set_text(memtotal, memavailable);
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

gchar	*hub_ram_set_text(guint64 memtotal, guint64 memavailable)
{
	gchar	*text;

	text = g_strdup_printf("RAM %.1f / %.1f GiB",
			(memtotal - memavailable) / (1024.0 * 1024.0),
			memtotal / (1024.0 * 1024.0));
	return (text);
}
