
#include "gaming_hub.h"

gchar	*hub_find_dir(const char *class_dir, const char *file, const char *value)
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
		folder = hub_find_dir_loop(value, entry, file, class_dir);
		if (folder)
			return (g_dir_close(dir), folder);
		entry = g_dir_read_name(dir);
	}
	g_dir_close(dir);
	return (NULL);
}

gchar	*hub_find_dir_loop(const char *value, const gchar *entry,
		const char *file_name, const char *class_dir)
{
	gchar	*name_path;
	gchar	*file;
	gchar	*folder;

	folder = g_build_filename(class_dir, entry, NULL);
	name_path = g_build_filename(folder, file_name, NULL);
	if (g_file_get_contents(name_path, &file, NULL, NULL))
	{
		g_strchomp(file);
		if (g_strcmp0(file, value) == 0)
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
