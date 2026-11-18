/* ************************************************************************** */
/*                                                                            */
/*                                            :::::::    :::     :::  :::::   */
/*   build.c                                  :+:   :+:  :+:+: :+:+:   :+:    */
/*                                            +#++:+#+   +#+ +:+ +#+   +#+    */
/*   By: TomSwb <github.com/TomSwb>           +#+   +#+  +#+     +#+   +#+    */
/*                                            ########   ###     ###  #####   */
/*   Created: 2026/11/17 17:59:15 by TomSwb                                   */
/*   Updated: 2026/11/17 17:59:15 by TomSwb                   bitminded.ch    */
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
	gtk_box_append(GTK_BOX(box), hub->stack);
	gtk_box_append(GTK_BOX(box), hub_build_stats(hub));
	hub_show_timer_page(hub);
	return (box);
}

GtkWidget	*hub_build_plan(t_hub *hub)
{
	GtkWidget	*box;
	GtkWidget	*box_left;
	GtkWidget	*box_right;
	GtkWidget	*label;

	box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 6);
	box_left = gtk_box_new(GTK_ORIENTATION_VERTICAL, 6);
	box_right = gtk_box_new(GTK_ORIENTATION_VERTICAL, 6);
	label = gtk_label_new("minutes");
	gtk_box_append(GTK_BOX(box_left), label);
	gtk_widget_set_halign(label, GTK_ALIGN_END);
	gtk_box_append(GTK_BOX(box_left),
		hub_labelled("Session length:", hub->timer.session));
	gtk_box_append(GTK_BOX(box_left),
		hub_labelled("Break every:", hub->timer.break_spin));
	gtk_box_append(GTK_BOX(box_left),
		hub_labelled("Wind down at:", hub->timer.wind_spin));
	gtk_widget_set_hexpand(box_left, TRUE);
	gtk_box_append(GTK_BOX(box_right),
		hub_image_with_label("alarm-symbolic", NULL));
	gtk_box_append(GTK_BOX(box_right), hub->timer.start);
	gtk_widget_set_halign(box_right, GTK_ALIGN_CENTER);
	gtk_box_append(GTK_BOX(box), box_left);
	gtk_box_append(GTK_BOX(box), box_right);
	gtk_widget_set_halign(box, GTK_ALIGN_CENTER);
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
	gtk_box_append(GTK_BOX(box), hub->timer.countdown);
	gtk_box_append(GTK_BOX(box), hub->timer.eye);
	gtk_box_append(GTK_BOX(box),
		hub_image_with_label("input-gaming-symbolic", NULL));
	gtk_box_append(GTK_BOX(box), box2);
	return (box);
}

GtkWidget	*hub_build_stats(t_hub *hub)
{
	GtkWidget	*box;

	box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 12);
	gtk_box_append(GTK_BOX(box), hub->computer.ram);
	gtk_box_append(GTK_BOX(box), hub->computer.cpu);
	gtk_box_append(GTK_BOX(box), hub->computer.temp);
	gtk_box_append(GTK_BOX(box), hub->computer.gpu);
	gtk_box_append(GTK_BOX(box), hub->computer.bat);
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
	return (box);
}
