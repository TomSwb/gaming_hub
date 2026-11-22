/* ************************************************************************** */
/*                                                                            */
/*                                            :::::::    :::     :::  :::::   */
/*   main_build.c                             :+:   :+:  :+:+: :+:+:   :+:    */
/*                                            +#++:+#+   +#+ +:+ +#+   +#+    */
/*   By: TomSwb <github.com/TomSwb>           +#+   +#+  +#+     +#+   +#+    */
/*                                            ########   ###     ###  #####   */
/*   Created: 2026/11/19 17:32:54 by TomSwb                                   */
/*   Updated: 2026/11/19 17:32:54 by TomSwb                   bitminded.ch    */
/*                                                                            */
/* ************************************************************************** */

#include "gaming_hub.h"

GtkWidget	*hub_build_box(t_hub *hub)
{
	GtkWidget	*box;
	GtkWidget	*stack;

	box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 12);
	stack = gtk_stack_new();
	hub->stack = stack;
	gtk_stack_add_named(GTK_STACK(stack), hub_build_plan(hub), "plan");
	gtk_stack_add_named(GTK_STACK(stack), hub_build_session(hub), "session");
	gtk_stack_add_named(GTK_STACK(stack), hub_build_break(hub), "break");
	gtk_stack_add_named(GTK_STACK(stack), hub_build_transit(hub), "transit");
	gtk_box_append(GTK_BOX(box), hub->stack);
	gtk_box_append(GTK_BOX(box), hub_build_headset(hub));
	gtk_box_append(GTK_BOX(box), hub_build_power(hub));
	gtk_box_append(GTK_BOX(box), hub_build_stats(hub));
	hub_show_timer_page(hub);
	return (box);
}

GtkWidget	*hub_image_with_label(const gchar *icon, const gchar *text)
{
	GtkWidget	*box;
	GtkWidget	*label;
	GtkWidget	*image;

	label = NULL;
	box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 8);
	image = gtk_image_new_from_icon_name(icon);
	gtk_image_set_pixel_size(GTK_IMAGE(image), 48);
	gtk_box_append(GTK_BOX(box), image);
	if (text)
	{
		label = gtk_label_new(text);
		gtk_label_set_wrap(GTK_LABEL(label), TRUE);
		gtk_box_append(GTK_BOX(box), label);
	}
	return (box);
}

GtkWidget	*hub_labelled(const gchar *text, GtkWidget *widget)
{
	GtkWidget	*box;
	GtkWidget	*label;

	box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 6);
	label = gtk_label_new(text);
	gtk_box_append(GTK_BOX(box), label);
	gtk_box_append(GTK_BOX(box), widget);
	gtk_widget_set_hexpand(label, TRUE);
	gtk_label_set_xalign(GTK_LABEL(label), 0);
	return (box);
}
