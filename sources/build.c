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

	box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 12);
	hub_build_plan(box, hub);
	hub_build_session(box, hub);
	hub_build_stats(box, hub);
	return (box);
}

void	hub_build_plan(GtkWidget *box, t_hub *hub)
{
	gtk_box_append(GTK_BOX(box), hub->timer.session);
	gtk_box_append(GTK_BOX(box), hub->timer.start);
}

void	hub_build_session(GtkWidget *box, t_hub *hub)
{
	gtk_box_append(GTK_BOX(box), hub->timer.countdown);
	gtk_box_append(GTK_BOX(box), hub->timer.pause);
	gtk_box_append(GTK_BOX(box), hub->timer.stop);
}

void	hub_build_stats(GtkWidget *box, t_hub *hub)
{
	gtk_box_append(GTK_BOX(box), hub->computer.ram);
	gtk_box_append(GTK_BOX(box), hub->computer.cpu);
	gtk_box_append(GTK_BOX(box), hub->computer.temp);
	gtk_box_append(GTK_BOX(box), hub->computer.gpu);
	gtk_box_append(GTK_BOX(box), hub->computer.bat);
}
