/* ************************************************************************** */
/*                                                                            */
/*                                            :::::::    :::     :::  :::::   */
/*   headset_build.c                          :+:   :+:  :+:+: :+:+:   :+:    */
/*                                            +#++:+#+   +#+ +:+ +#+   +#+    */
/*   By: TomSwb <github.com/TomSwb>           +#+   +#+  +#+     +#+   +#+    */
/*                                            ########   ###     ###  #####   */
/*   Created: 2026/11/21 17:56:16 by TomSwb                                   */
/*   Updated: 2026/11/21 17:56:16 by TomSwb                   bitminded.ch    */
/*                                                                            */
/* ************************************************************************** */

#include "gaming_hub.h"

GtkWidget	*hub_build_headset(t_hub *hub)
{
	GtkWidget	*box;
	GtkWidget	*label;

	box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 8);
	label = gtk_label_new("Headset");
	hub->headset.volume = gtk_label_new("Volume: off");
	hub->headset.mic = gtk_label_new("Mic: off");
	hub_build_sliders(hub);
	hub_build_mute(hub);
	gtk_box_append(GTK_BOX(box), label);
	gtk_box_append(GTK_BOX(box), hub->headset.volume);
	gtk_box_append(GTK_BOX(box), hub->headset.sld_vol);
	gtk_box_append(GTK_BOX(box), hub->headset.mute_vol);
	gtk_box_append(GTK_BOX(box), hub->headset.mic);
	gtk_box_append(GTK_BOX(box), hub->headset.sld_mic);
	gtk_box_append(GTK_BOX(box), hub->headset.mute_mic);
	return (box);
}

void	hub_build_sliders(t_hub *hub)
{
	hub->headset.sld_vol = gtk_scale_new_with_range(GTK_ORIENTATION_HORIZONTAL,
			0, 100, 1);
	gtk_widget_set_hexpand(hub->headset.sld_vol, TRUE);
	gtk_widget_set_sensitive(hub->headset.sld_vol, FALSE);
	g_signal_connect(hub->headset.sld_vol, "value-changed",
			G_CALLBACK(hub_sink_slider_moved), hub);
	hub->headset.sld_mic = gtk_scale_new_with_range(GTK_ORIENTATION_HORIZONTAL,
			0, 100, 1);
	gtk_widget_set_hexpand(hub->headset.sld_mic, TRUE);
	gtk_widget_set_sensitive(hub->headset.sld_mic, FALSE);
	g_signal_connect(hub->headset.sld_mic, "value-changed",
			G_CALLBACK(hub_source_slider_moved), hub);
}

void	hub_build_mute(t_hub *hub)
{
	hub->headset.mute_vol = gtk_toggle_button_new_with_label("Mute");
	gtk_widget_set_sensitive(hub->headset.mute_vol, FALSE);
	g_signal_connect(hub->headset.mute_vol, "toggled",
			G_CALLBACK(hub_sink_mute_toggled), hub);
	hub->headset.mute_mic = gtk_toggle_button_new_with_label("Mute");
	gtk_widget_set_sensitive(hub->headset.mute_mic, FALSE);
	g_signal_connect(hub->headset.mute_mic, "toggled",
			G_CALLBACK(hub_source_mute_toggled), hub);
}

