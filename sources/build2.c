/* ************************************************************************** */
/*                                                                            */
/*                                            :::::::    :::     :::  :::::   */
/*   build2.c                                 :+:   :+:  :+:+: :+:+:   :+:    */
/*                                            +#++:+#+   +#+ +:+ +#+   +#+    */
/*   By: TomSwb <github.com/TomSwb>           +#+   +#+  +#+     +#+   +#+    */
/*                                            ########   ###     ###  #####   */
/*   Created: 2026/11/17 18:44:54 by TomSwb                                   */
/*   Updated: 2026/11/17 18:44:54 by TomSwb                   bitminded.ch    */
/*                                                                            */
/* ************************************************************************** */

#include "gaming_hub.h"

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
	gtk_box_append(GTK_BOX(box), hub->timer.done);
	return (box);
}

GtkWidget	*hub_image_with_label(const gchar *icon, const gchar *text)
{
	GtkWidget	*box;
	GtkWidget	*label;
	GtkWidget	*image;

	box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 8);
	image = gtk_image_new_from_file(icon);
	label = gtk_label_new(text);
	gtk_box_append(GTK_BOX(box), image);
	gtk_box_append(GTK_BOX(box), label);
	return (box);
}
