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
	hub_headset_build_box(hub, box, label);
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

void	hub_headset_build_box(t_hub *hub, GtkWidget *box, GtkWidget *label)
{
	GtkWidget	*box1;
	GtkWidget	*box_title;
	GtkWidget	*box2;
	GtkWidget	*box3;
	GtkWidget	*box4;

	box1 = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 8);
	box_title = gtk_box_new(GTK_ORIENTATION_VERTICAL, 8);
	box2 = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 8);
	box3 = gtk_box_new(GTK_ORIENTATION_VERTICAL, 8);
	box4 = gtk_box_new(GTK_ORIENTATION_VERTICAL, 8);
	gtk_box_append(GTK_BOX(box_title), label);
	gtk_box_append(GTK_BOX(box3), hub->headset.volume);
	gtk_box_append(GTK_BOX(box3), hub->headset.sld_vol);
	gtk_box_append(GTK_BOX(box3), hub->headset.mute_vol);
	gtk_box_append(GTK_BOX(box4), hub->headset.mic);
	gtk_box_append(GTK_BOX(box4), hub->headset.sld_mic);
	gtk_box_append(GTK_BOX(box4), hub->headset.mute_mic);
	gtk_box_append(GTK_BOX(box1), box_title);
	gtk_box_append(GTK_BOX(box2), box3);
	gtk_box_append(GTK_BOX(box2), box4);
	gtk_box_append(GTK_BOX(box), box1);
	gtk_box_append(GTK_BOX(box), box2);
	gtk_widget_set_halign(box1, GTK_ALIGN_CENTER);
}
