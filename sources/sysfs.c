
#include "gaming_hub.h"

gchar	*hub_find_hwmon(const char *sensor)
{
	GDir		*dir;
	const gchar	*entry;
	gchar		*folder;

	dir = g_dir_open("/sys/class/hwmon", 0, NULL);
	if (!dir)
		return (NULL);
	entry = g_dir_read_name(dir);
	while (entry)
	{
		folder = hub_find_hwmon_loop(sensor, entry);
		if (folder)
			return (g_dir_close(dir), folder);
		entry = g_dir_read_name(dir);
	}
	g_dir_close(dir);
	return (NULL);
}

gchar	*hub_find_hwmon_loop(const char *sensor, const gchar *entry)
{
	gchar	*name_path;
	gchar	*file;
	gchar	*folder;

	folder = g_build_filename("/sys/class/hwmon", entry, NULL);
	name_path = g_build_filename(folder, "name", NULL);
	if (g_file_get_contents(name_path, &file, NULL, NULL))
	{
		g_strchomp(file);
		if (g_strcmp0(file, sensor) == 0)
		{
			g_free(name_path);
			g_free(file);
			return (folder);
		}
		g_free(file);
	}
	g_free(name_path);
	g_free(folder);
	return (NULL);
}

gchar	*hub_find_gpu(void)
{
	GDir		*dir;
	const gchar	*entry;
	gchar		*folder;
	gchar		*name_path;

	dir = g_dir_open("/sys/class/drm", 0, NULL);
	if (!dir)
		return (NULL);
	entry = g_dir_read_name(dir);
	while (entry)
	{
		folder = g_build_filename("/sys/class/drm", entry, NULL);
		name_path = g_build_filename(folder, "gt_act_freq_mhz", NULL);
		if (g_file_test(name_path, G_FILE_TEST_EXISTS))
		{
			g_free(name_path);
			return (g_dir_close(dir), folder);
		}
		g_free(folder);
		g_free(name_path);
		entry = g_dir_read_name(dir);
	}
	g_dir_close(dir);
	return (NULL);
}

gboolean	hub_read_number(const gchar *dir, const gchar *file,
				guint64 *value)
{
	gchar	*path;
	gchar	*contents;
	gchar	*end;

	if (!dir)
		return (FALSE);
	path = g_build_filename(dir, file, NULL);
	if (!g_file_get_contents(path, &contents, NULL, NULL))
	{
		g_free(path);
		return (FALSE);
	}
	*value = g_ascii_strtoull(contents, &end, 10);
	if (end == contents)
	{
		g_free(contents);
		g_free(path);
		return (FALSE);
	}
	g_free(contents);
	g_free(path);
	return (TRUE);
}
