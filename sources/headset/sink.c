/* ************************************************************************** */
/*                                                                            */
/*                                            :::::::    :::     :::  :::::   */
/*   sink.c                                   :+:   :+:  :+:+: :+:+:   :+:    */
/*                                            +#++:+#+   +#+ +:+ +#+   +#+    */
/*   By: TomSwb <github.com/TomSwb>           +#+   +#+  +#+     +#+   +#+    */
/*                                            ########   ###     ###  #####   */
/*   Created: 2026/11/21 17:15:42 by TomSwb                                   */
/*   Updated: 2026/11/21 17:15:42 by TomSwb                   bitminded.ch    */
/*                                                                            */
/* ************************************************************************** */

#include "gaming_hub.h"

void	hub_sink_list(pa_context *c, const pa_sink_info *i, int eol,
			void *user_data)
{
	t_hub	*hub;

	(void)c;
	hub = user_data;
	if (eol < 0)
		g_print("Pulse: Error sink line\n");
	else if (eol == 0)
	{
		if (g_strcmp0(i->description, HUB_HEADSET_NAME) == 0)
		{
			hub->headset.sink_index = i->index;
			g_print("Pulse: Headset Found\n");
			hub_show_sink(hub, i);
		}
	}
	else if (eol > 0 && hub->headset.sink_index == PA_INVALID_INDEX)
		g_print("Pulse: No headset - end of list\n");
	else
		g_print("Pulse: End of list\n");
}

void	hub_show_sink(t_hub *hub, const pa_sink_info *i)
{
	gchar	*text;

	if (i == NULL)
	{
		text = g_strdup("Headset: off\n");
		gtk_label_set_text(GTK_LABEL(hub->headset.headset),
		text);
	}
	else
	{
		if (i->mute == 0)
			text = g_strdup_printf("Headset: %d%%\n",
				hub_volume_percent(&i->volume));
		else
		 	text = g_strdup_printf("Headset: %d%% (muted)\n",
				hub_volume_percent(&i->volume));
		gtk_label_set_text(GTK_LABEL(hub->headset.headset),
		text);
	}
	g_free(text);
}
