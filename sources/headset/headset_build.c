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

	box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 8);
	hub->headset.headset = gtk_label_new("Headset: ...");
	gtk_box_append(GTK_BOX(box), hub->headset.headset);
	return (box);
}
