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
	gtk_widget_set_halign(icon_row, GTK_ALIGN_CENTER);
	gtk_widget_set_valign(icon_row, GTK_ALIGN_CENTER);
	gtk_box_set_homogeneous(GTK_BOX(icon_row), TRUE);
	gtk_box_append(GTK_BOX(box), icon_row);
	gtk_box_append(GTK_BOX(box), hub->timer.done);
	gtk_widget_set_halign(hub->timer.done, GTK_ALIGN_CENTER);
	gtk_widget_set_valign(hub->timer.done, GTK_ALIGN_CENTER);
	gtk_widget_set_valign(box, GTK_ALIGN_CENTER);
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
