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
		g_printerr("pulse: Error sink line\n");
	else if (eol == 0)
	{
		if (g_strcmp0(i->name, hub->headset.def_sink_name) == 0)
		{
			hub->headset.sink_index = i->index;
			hub_show_sink(hub, i);
		}
	}
	else if (hub->headset.sink_index == PA_INVALID_INDEX)
		hub_show_sink(hub, NULL);
}

void	hub_show_sink(t_hub *hub, const pa_sink_info *i)
{
	gchar	*text;
	gint	per_vol;

	if (i == NULL)
	{
		text = g_strdup("No device found ...");
		gtk_widget_set_sensitive(hub->headset.sld_vol, FALSE);
		gtk_widget_set_sensitive(hub->headset.mute_vol, FALSE);
	}
	else
	{
		per_vol = hub_volume_percent(&i->volume);
		hub_sink_set_buttons(hub, per_vol, i);
		if (i->mute == 0)
			text = g_strdup_printf("Volume: %d%%", per_vol);
		else
			text = g_strdup_printf("Volume: %d%% (muted)", per_vol);
	}
	gtk_label_set_text(GTK_LABEL(hub->headset.volume), text);
	g_free(text);
}

void	hub_sink_set_buttons(t_hub *hub, gint per_vol,
			const pa_sink_info *i)
{
	hub->headset.sink_vol = i->volume;
	g_signal_handlers_block_by_func(hub->headset.sld_vol,
		G_CALLBACK(hub_sink_slider_moved), hub);
	if (g_get_monotonic_time() - hub->headset.moved_vol > 300000)
		gtk_range_set_value(GTK_RANGE(hub->headset.sld_vol), per_vol);
	gtk_widget_set_sensitive(hub->headset.sld_vol, TRUE);
	g_signal_handlers_unblock_by_func(hub->headset.sld_vol,
		G_CALLBACK(hub_sink_slider_moved), hub);
	g_signal_handlers_block_by_func(hub->headset.mute_vol,
		G_CALLBACK(hub_sink_mute_toggled), hub);
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(hub->headset.mute_vol),
		i->mute);
	gtk_widget_set_sensitive(hub->headset.mute_vol, TRUE);
	g_signal_handlers_unblock_by_func(hub->headset.mute_vol,
		G_CALLBACK(hub_sink_mute_toggled), hub);
}

void	hub_sink_slider_moved(GtkRange *self, gpointer user_data)
{
	t_hub			*hub;
	double			value;
	pa_cvolume		volume;
	pa_volume_t		new_max;
	pa_operation	*op;

	hub = user_data;
	if (hub->headset.sink_index == PA_INVALID_INDEX)
		return ;
	hub->headset.moved_vol = g_get_monotonic_time();
	value = gtk_range_get_value(self);
	volume = hub->headset.sink_vol;
	new_max = ((value * PA_VOLUME_NORM) + 50) / 100;
	pa_cvolume_scale(&volume, new_max);
	op = pa_context_set_sink_volume_by_index(hub->headset.context,
			hub->headset.sink_index, &volume, NULL, NULL);
	if (op)
		pa_operation_unref(op);
	op = pa_context_set_sink_mute_by_index(hub->headset.context,
			hub->headset.sink_index, new_max == 0, NULL, NULL);
	if (op)
		pa_operation_unref(op);
}

void	hub_sink_mute_toggled(GtkToggleButton *self, gpointer user_data)
{
	t_hub			*hub;
	gboolean		state;
	pa_operation	*op;

	hub = user_data;
	if (hub->headset.sink_index == PA_INVALID_INDEX)
		return ;
	state = gtk_toggle_button_get_active(self);
	op = pa_context_set_sink_mute_by_index(hub->headset.context,
			hub->headset.sink_index, state, NULL, NULL);
	if (op)
		pa_operation_unref(op);
}
