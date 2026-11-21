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
	gint	per_mic;

	if (i == NULL)
	{
		text = g_strdup("No device found ...");
		gtk_widget_set_sensitive(hub->headset.sld_mic, FALSE);
	}
	else
	{
		per_mic = hub_volume_percent(&i->volume);
		hub->headset.src_vol = i->volume;
		gtk_range_set_value(GTK_RANGE(hub->headset.sld_mic), per_mic);
		gtk_widget_set_sensitive(hub->headset.sld_mic, TRUE);
		if (i->mute == 0)
			text = g_strdup_printf("Mic: %d%%", per_mic);
		else
			text = g_strdup_printf("Mic: %d%% (muted)", per_mic);
	}
	gtk_label_set_text(GTK_LABEL(hub->headset.mic),
		text);
	g_free(text);
}

void	hub_source_slider_moved(GtkRange *self, gpointer user_data)
{
	t_hub			*hub;
	double			value;
	pa_cvolume		volume;
	pa_volume_t		new_max;
	pa_operation	*op;

	hub = user_data;
	if (hub->headset.sink_index == PA_INVALID_INDEX)
		return ;
	value = gtk_range_get_value(self);
	volume = hub->headset.src_vol;
	new_max = ((value * PA_VOLUME_NORM) + 50) / 100;
	pa_cvolume_scale(&volume, new_max);
	op = pa_context_set_sink_volume_by_index(hub->headset.context,
			hub->headset.sink_index, &volume, NULL, NULL);
	if (op)
		pa_operation_unref(op);
}
