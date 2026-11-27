/* ************************************************************************** */
/*                                                                            */
/*                                            :::::::    :::     :::  :::::   */
/*   launch_argv.c                            :+:   :+:  :+:+: :+:+:   :+:    */
/*                                            +#++:+#+   +#+ +:+ +#+   +#+    */
/*   By: TomSwb <github.com/TomSwb>           +#+   +#+  +#+     +#+   +#+    */
/*                                            ########   ###     ###  #####   */
/*   Created: 2026/11/27 08:40:52 by TomSwb                                   */
/*   Updated: 2026/11/27 08:40:52 by TomSwb                   bitminded.ch    */
/*                                                                            */
/* ************************************************************************** */

#include "gaming_hub.h"

gchar	**hub_launch_argv(const t_hub_game *game)
{
	gchar	**words;
	gint	argc;
	GError	*error;
	gchar	**argv;

	error = NULL;
	if (!g_shell_parse_argv(game->command, &argc, &words, &error))
	{
		g_printerr("%s: %s\n", game->name, error->message);
		g_error_free(error);
		return (NULL);
	}
	argv = g_new0(gchar *, argc + 2);
	hub_launch_fill(argv, words, game->path);
	g_strfreev(words);
	return (argv);
}

void	hub_launch_fill(gchar **argv, gchar **words, const gchar *path)
{
	gint	i;

	i = 0;
	argv[0] = g_strdup(HUB_GAMEMODE);
	while (words[i])
	{
		if (g_strcmp0(HUB_FILE_MARK, words[i]) == 0)
			argv[i + 1] = g_strdup(path);
		else
		 	argv[i + 1] = g_strdup(words[i]);
		i++;
	}
}
