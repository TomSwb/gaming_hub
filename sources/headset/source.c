/* ************************************************************************** */
/*                                                                            */
/*                                            :::::::    :::     :::  :::::   */
/*   source.c                                 :+:   :+:  :+:+: :+:+:   :+:    */
/*                                            +#++:+#+   +#+ +:+ +#+   +#+    */
/*   By: TomSwb <github.com/TomSwb>           +#+   +#+  +#+     +#+   +#+    */
/*                                            ########   ###     ###  #####   */
/*   Created: 2026/11/21 18:26:14 by TomSwb                                   */
/*   Updated: 2026/11/21 18:26:14 by TomSwb                   bitminded.ch    */
/*                                                                            */
/* ************************************************************************** */

#include "gaming_hub.h"

void	hub_source_list(pa_context *c, const pa_source_info *i, int eol,
			void *user_data)
{
	t_hub	*hub;

	(void)c;
	hub = user_data;
	if (eol < 0)
		g_print("Pulse: Error source line\n");
	else if (eol == 0)
	{
		if (g_strcmp0(i->description, HUB_HEADSET_NAME) == 0)
		{
			hub->headset.source_index = i->index;
			g_print("Pulse: Mic Found\n");
			hub_show_source(hub, i);
		}
	}
	else if (eol > 0 && hub->headset.source_index == PA_INVALID_INDEX)
		hub_show_source(hub, NULL);
	else
		g_print("Pulse: End of list\n");
}

void	hub_show_source(t_hub *hub, const pa_source_info *i)
{
	gchar	*text;

	if (i == NULL)
		text = g_strdup("No device found ...");
	else
	{
		if (i->mute == 0)
			text = g_strdup_printf("Mic: %d%%",
				hub_volume_percent(&i->volume));
		else
			text = g_strdup_printf("Mic: %d%% (muted)",
				hub_volume_percent(&i->volume));
	}
	gtk_label_set_text(GTK_LABEL(hub->headset.mic),
		text);
	g_free(text);
}
