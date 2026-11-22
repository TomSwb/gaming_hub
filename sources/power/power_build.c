/* ************************************************************************** */
/*                                                                            */
/*                                            :::::::    :::     :::  :::::   */
/*   power_build.c                            :+:   :+:  :+:+: :+:+:   :+:    */
/*                                            +#++:+#+   +#+ +:+ +#+   +#+    */
/*   By: TomSwb <github.com/TomSwb>           +#+   +#+  +#+     +#+   +#+    */
/*                                            ########   ###     ###  #####   */
/*   Created: 2026/11/22 10:34:00 by TomSwb                                   */
/*   Updated: 2026/11/22 10:34:00 by TomSwb                   bitminded.ch    */
/*                                                                            */
/* ************************************************************************** */

#include "gaming_hub.h"

GtkWidget	*hub_build_power(t_hub *hub)
{
	GtkWidget	*box;

	box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 12);
	hub->power.profile = gtk_label_new("Power profile: ...");
	hub->power.pow_but_row = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 12);
	gtk_widget_add_css_class(hub->power.pow_but_row, "linked");
	gtk_box_append(GTK_BOX(box), hub->power.profile);
	gtk_box_append(GTK_BOX(box), hub->power.pow_but_row);
	gtk_widget_set_halign(hub->power.pow_but_row, GTK_ALIGN_CENTER);
	return (box);
}
