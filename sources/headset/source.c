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
		g_printerr("pulse: Error source line\n");
	else if (eol == 0)
	{
		if (g_strcmp0(i->description, HUB_HEADSET_NAME) == 0)
		{
			hub->headset.src_index = i->index;
			g_print("pulse: Mic Found\n");
			hub_show_source(hub, i);
		}
	}
	else if (hub->headset.src_index == PA_INVALID_INDEX)
		hub_show_source(hub, NULL);
}

void	hub_show_source(t_hub *hub, const pa_source_info *i)
{
	gchar	*text;
	gint	per_mic;

	if (i == NULL)
	{
		text = g_strdup("No device found ...");
		gtk_widget_set_sensitive(hub->headset.sld_mic, FALSE);
		gtk_widget_set_sensitive(hub->headset.mute_mic, FALSE);
	}
	else
	{
		per_mic = hub_volume_percent(&i->volume);
		hub_source_set_buttons(hub, per_mic, i);
		if (i->mute == 0)
			text = g_strdup_printf("Mic: %d%%", per_mic);
		else
			text = g_strdup_printf("Mic: %d%% (muted)", per_mic);
	}
	gtk_label_set_text(GTK_LABEL(hub->headset.mic), text);
	g_free(text);
}

void	hub_source_set_buttons(t_hub *hub, gint per_mic,
			const pa_source_info *i)
{
	hub->headset.src_vol = i->volume;
	g_signal_handlers_block_by_func(hub->headset.sld_mic,
		G_CALLBACK(hub_source_slider_moved), hub);
	if (g_get_monotonic_time() - hub->headset.moved_mic > 300000)
		gtk_range_set_value(GTK_RANGE(hub->headset.sld_mic), per_mic);
	gtk_widget_set_sensitive(hub->headset.sld_mic, TRUE);
	g_signal_handlers_unblock_by_func(hub->headset.sld_mic,
		G_CALLBACK(hub_source_slider_moved), hub);
	g_signal_handlers_block_by_func(hub->headset.mute_mic,
		G_CALLBACK(hub_source_mute_toggled), hub);
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(hub->headset.mute_mic),
		i->mute);
	gtk_widget_set_sensitive(hub->headset.mute_mic, TRUE);
	g_signal_handlers_unblock_by_func(hub->headset.mute_mic,
		G_CALLBACK(hub_source_mute_toggled), hub);
}

void	hub_source_slider_moved(GtkRange *self, gpointer user_data)
{
	t_hub			*hub;
	double			value;
	pa_cvolume		volume;
	pa_volume_t		new_max;
	pa_operation	*op;

	hub = user_data;
	if (hub->headset.src_index == PA_INVALID_INDEX)
		return ;
	hub->headset.moved_mic = g_get_monotonic_time();
	value = gtk_range_get_value(self);
	volume = hub->headset.src_vol;
	new_max = ((value * PA_VOLUME_NORM) + 50) / 100;
	pa_cvolume_scale(&volume, new_max);
	op = pa_context_set_source_volume_by_index(hub->headset.context,
			hub->headset.src_index, &volume, NULL, NULL);
	if (op)
		pa_operation_unref(op);
	op = pa_context_set_source_mute_by_index(hub->headset.context,
			hub->headset.src_index, new_max == 0, NULL, NULL);
	if (op)
		pa_operation_unref(op);
}

void	hub_source_mute_toggled(GtkToggleButton *self, gpointer user_data)
{
	t_hub			*hub;
	gboolean		state;
	pa_operation	*op;

	hub = user_data;
	if (hub->headset.src_index == PA_INVALID_INDEX)
		return ;
	state = gtk_toggle_button_get_active(self);
	op = pa_context_set_source_mute_by_index(hub->headset.context,
			hub->headset.src_index, state, NULL, NULL);
	if (op)
		pa_operation_unref(op);
}
