/* ************************************************************************** */
/*                                                                            */
/*                                            :::::::    :::     :::  :::::   */
/*   timer_build.c                            :+:   :+:  :+:+: :+:+:   :+:    */
/*                                            +#++:+#+   +#+ +:+ +#+   +#+    */
/*   By: TomSwb <github.com/TomSwb>           +#+   +#+  +#+     +#+   +#+    */
/*                                            ########   ###     ###  #####   */
/*   Created: 2026/11/17 17:59:15 by TomSwb                                   */
/*   Updated: 2026/11/17 17:59:15 by TomSwb                   bitminded.ch    */
/*                                                                            */
/* ************************************************************************** */

#include "gaming_hub.h"

GtkWidget	*hub_build_plan(t_hub *hub)
{
	GtkWidget	*box;
	GtkWidget	*box_left;
	GtkWidget	*box_middle;
	GtkWidget	*box_right;

	box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 6);
	box_right = gtk_box_new(GTK_ORIENTATION_VERTICAL, 6);
	box_left = gtk_box_new(GTK_ORIENTATION_VERTICAL, 6);
	gtk_box_append(GTK_BOX(box_right),
		hub_image_with_label("alarm-symbolic", NULL));
	gtk_box_append(GTK_BOX(box_right), hub->timer.start);
	gtk_widget_set_halign(box_right, GTK_ALIGN_CENTER);
	gtk_widget_set_valign(box_right, GTK_ALIGN_CENTER);
	gtk_box_append(GTK_BOX(box_left), hub->timer.popups_check);
	gtk_box_append(GTK_BOX(box_left), hub->timer.sound_check);
	gtk_widget_set_halign(box_left, GTK_ALIGN_CENTER);
	gtk_widget_set_valign(box_left, GTK_ALIGN_CENTER);
	box_middle = hub_plan_box_middle(hub);
	gtk_box_append(GTK_BOX(box), box_left);
	gtk_box_append(GTK_BOX(box), box_middle);
	gtk_box_append(GTK_BOX(box), box_right);
	gtk_box_set_homogeneous(GTK_BOX(box), TRUE);
	return (box);
}

GtkWidget	*hub_plan_box_middle(t_hub *hub)
{
	GtkWidget	*box;

	box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 6);
	gtk_box_append(GTK_BOX(box),
		hub_labelled("Session length:", hub->timer.session));
	gtk_box_append(GTK_BOX(box),
		hub_labelled("Break every:", hub->timer.break_spin));
	gtk_box_append(GTK_BOX(box),
		hub_labelled("Wind down at:", hub->timer.wind_spin));
	gtk_widget_set_halign(box, GTK_ALIGN_END);
	gtk_widget_set_valign(box, GTK_ALIGN_CENTER);
	return (box);
}

GtkWidget	*hub_build_session(t_hub *hub)
{
	GtkWidget	*box;
	GtkWidget	*box2;

	box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 6);
	box2 = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 12);
	gtk_box_append(GTK_BOX(box2), hub->timer.pause);
	gtk_box_append(GTK_BOX(box2), hub->timer.stop);
	gtk_widget_set_halign(box2, GTK_ALIGN_CENTER);
	gtk_widget_set_valign(box2, GTK_ALIGN_CENTER);
	gtk_box_append(GTK_BOX(box), hub->timer.countdown);
	gtk_box_append(GTK_BOX(box), hub->timer.eye);
	gtk_box_append(GTK_BOX(box),
		hub_image_with_label("input-gaming-symbolic", NULL));
	gtk_box_append(GTK_BOX(box), box2);
	gtk_box_set_homogeneous(GTK_BOX(box), TRUE);
	return (box);
}

GtkWidget	*hub_build_break(t_hub *hub)
{
	GtkWidget	*box;
	GtkWidget	*icon_row;
	GtkWidget	*label;

	box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 8);
	label = gtk_label_new("Break");
	gtk_box_append(GTK_BOX(box), label);
	icon_row = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 12);
	gtk_box_append(GTK_BOX(icon_row),
		hub_image_with_label("preferences-desktop-accessibility-symbolic",
				"Stand"));
	gtk_box_append(GTK_BOX(icon_row),
		hub_image_with_label("object-rotate-right-symbolic",
				"Roll neck & shoulder"));
	gtk_box_append(GTK_BOX(icon_row),
		hub_image_with_label("weather-showers-symbolic", "Drink water"));
	gtk_widget_set_halign(icon_row, GTK_ALIGN_CENTER);
	gtk_box_set_homogeneous(GTK_BOX(icon_row), TRUE);
	gtk_box_append(GTK_BOX(box), icon_row);
	gtk_box_append(GTK_BOX(box), hub->timer.done);
	gtk_widget_set_halign(hub->timer.done, GTK_ALIGN_CENTER);
	gtk_widget_set_valign(box, GTK_ALIGN_CENTER);
	return (box);
}
