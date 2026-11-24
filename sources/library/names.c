/* ************************************************************************** */
/*                                                                            */
/*                                            :::::::    :::     :::  :::::   */
/*   names.c                                  :+:   :+:  :+:+: :+:+:   :+:    */
/*                                            +#++:+#+   +#+ +:+ +#+   +#+    */
/*   By: TomSwb <github.com/TomSwb>           +#+   +#+  +#+     +#+   +#+    */
/*                                            ########   ###     ###  #####   */
/*   Created: 2026/11/24 14:32:09 by TomSwb                                   */
/*   Updated: 2026/11/24 14:32:09 by TomSwb                   bitminded.ch    */
/*                                                                            */
/* ************************************************************************** */

#include "gaming_hub.h"

void	hub_name_clear(t_dump_name *dump)
{
	g_free(dump->title);
	g_free(dump->region);
	g_free(dump->id);
	dump->title = NULL;
	dump->region = NULL;
	dump->id = NULL;
}

gboolean	hub_name_split(const gchar *file, gsize len, t_dump_name *dump)
{
	const gchar	*bracket;
	const gchar	*paren_end;
	const gchar	*start;

	if (len < 2 || file[len - 1] != ']')
		return (FALSE);
	bracket = g_strrstr_len(file, len, "[");
	if (!bracket || bracket - file < 2)
		return (FALSE);
	if (bracket[-1] != ' ' || bracket[-2] != ')')
		return (FALSE);
	paren_end = bracket - 2;
	start = g_strrstr_len(file, paren_end - file, "(");
	if (!start || start == file)
		return (FALSE);
	dump->id = g_strndup(bracket + 1, (file + len - 1) - (bracket + 1));
	dump->region = g_strndup(start + 1, paren_end - (start + 1));
	dump->title = g_strndup(file, start - file);
	g_strstrip(dump->title);
	return (TRUE);
}

gboolean	hub_name_parse(const gchar *file, t_dump_name *dump)
{
	const gchar	*dot;
	gsize		len;

	ft_bzero(dump, sizeof(t_dump_name));
	dot = ft_strrchr(file, '.');
	if (dot && dot != file)
		len = dot - file;
	else
		len = ft_strlen(file);
	if (hub_name_split(file, len, dump))
		return (TRUE);
	dump->title = g_strndup(file, len);
	return (FALSE);
}
