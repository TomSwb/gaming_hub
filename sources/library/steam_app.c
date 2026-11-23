/* ************************************************************************** */
/*                                                                            */
/*                                            :::::::    :::     :::  :::::   */
/*   steam_app.c                              :+:   :+:  :+:+: :+:+:   :+:    */
/*                                            +#++:+#+   +#+ +:+ +#+   +#+    */
/*   By: TomSwb <github.com/TomSwb>           +#+   +#+  +#+     +#+   +#+    */
/*                                            ########   ###     ###  #####   */
/*   Created: 2026/11/23 20:31:29 by TomSwb                                   */
/*   Updated: 2026/11/23 20:31:29 by TomSwb                   bitminded.ch    */
/*                                                                            */
/* ************************************************************************** */

#include "gaming_hub.h"

void	hub_steam_app(GListStore *store, const gchar *folder,
			const gchar *name)
{
	gchar	*file;

	(void)store;
	file = g_build_filename(folder, name, NULL);
	g_print("path: %s\n", file);
	g_free(file);
}
