/* ************************************************************************** */
/*                                                                            */
/*                                            :::::::    :::     :::  :::::   */
/*   disc.c                                   :+:   :+:  :+:+: :+:+:   :+:    */
/*                                            +#++:+#+   +#+ +:+ +#+   +#+    */
/*   By: TomSwb <github.com/TomSwb>           +#+   +#+  +#+     +#+   +#+    */
/*                                            ########   ###     ###  #####   */
/*   Created: 2026/11/24 17:58:15 by TomSwb                                   */
/*   Updated: 2026/11/24 17:58:15 by TomSwb                   bitminded.ch    */
/*                                                                            */
/* ************************************************************************** */

#include "gaming_hub.h"

gboolean	hub_disc_id(const gchar *path, gchar *id)
{
	int				fd;
	unsigned char	header[HUB_DISC_HEADER];
	ssize_t			got;

	fd = open(path, O_RDONLY);
	if (fd < 0)
		return (FALSE);
	got = read(fd, header, HUB_DISC_HEADER);
	close(fd);
	if (got != HUB_DISC_HEADER)
		return (FALSE);
	if (ft_memcmp(header + HUB_WII_OFFSET, HUB_WII_MAGIC, HUB_MAGIC_LEN)
		&& ft_memcmp(header + HUB_GC_OFFSET, HUB_GC_MAGIC, HUB_MAGIC_LEN))
		return (FALSE);
	ft_memcpy(id, header, HUB_DISC_ID_LEN);
	id[HUB_DISC_ID_LEN] = '\0';
	return (TRUE);
}

void	hub_disc_check(const gchar *path, t_dump_name *dump)
{
	gchar	id[HUB_DISC_ID_LEN + 1];

	if (!hub_disc_id(path, id))
		return ;
	if (!dump->id)
	{
		dump->id = g_strdup(id);
		return ;
	}
	if (g_strcmp0(dump->id, id) != 0)
		g_printerr("%s: the name says [%s], the disc says [%s]\n",
			path, dump->id, id);
}
