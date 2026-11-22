/* ************************************************************************** */
/*                                                                            */
/*                                            :::::::    :::     :::  :::::   */
/*   outputs.c                                :+:   :+:  :+:+: :+:+:   :+:    */
/*                                            +#++:+#+   +#+ +:+ +#+   +#+    */
/*   By: TomSwb <github.com/TomSwb>           +#+   +#+  +#+     +#+   +#+    */
/*                                            ########   ###     ###  #####   */
/*   Created: 2026/11/22 21:41:26 by TomSwb                                   */
/*   Updated: 2026/11/22 21:41:26 by TomSwb                   bitminded.ch    */
/*                                                                            */
/* ************************************************************************** */

#include "gaming_hub.h"

void	hub_outputs_refresh(t_hub *hub)
{
	pa_operation	*op;
	GtkWidget		*child;

	child = gtk_widget_get_first_child(hub->headset.dev_row);
	while (child)
	{
		gtk_box_remove(GTK_BOX(hub->headset.dev_row), child);
		child = gtk_widget_get_first_child(hub->headset.dev_row);
	}
	op = pa_context_get_sink_info_list(hub->headset.context,
			hub_outputs_list, hub);
	if (op)
		pa_operation_unref(op);
}

void	hub_outputs_list(pa_context *c, const pa_sink_info *i, int eol,
			void *user_data)
{
	(void)c;
	if (eol > 0)
	{
		hub_outputs_select(user_data);
		return ;
	}
	else if (eol < 0 || !i)
		return ;
	if (i->active_port && i->active_port->available == PA_PORT_AVAILABLE_NO)
		return ;
	hub_outputs_add(user_data, i);
}

const gchar	*hub_sink_short_name(const pa_sink_info *i)
{
	const gchar	*name;

	name = pa_proplist_gets(i->proplist, HUB_PA_SHORT_NAME);
	if (!name)
		name = i->description;
	return (name);
}

void	hub_outputs_add(t_hub *hub, const pa_sink_info *i)
{
	GtkWidget	*leader;
	GtkWidget	*button;

	button = gtk_toggle_button_new_with_label(hub_sink_short_name(i));
	g_object_set_data_full(G_OBJECT(button), HUB_PA_KEY,
		g_strdup(i->name), g_free);
	leader = gtk_widget_get_first_child(hub->headset.dev_row);
	if (leader)
		gtk_toggle_button_set_group(GTK_TOGGLE_BUTTON(button),
			GTK_TOGGLE_BUTTON(leader));
	gtk_box_append(GTK_BOX(hub->headset.dev_row), button);
}

void	hub_outputs_select(t_hub *hub)
{
	GtkWidget	*child;
	const gchar	*name;

	child = gtk_widget_get_first_child(hub->power.pow_but_row);
	while (child != NULL)
	{
		name = g_object_get_data(G_OBJECT(child), HUB_PA_KEY);
		// g_signal_handlers_block_by_func(child,
		// 	G_CALLBACK(hub_power_toggled), hub);
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(child),
			g_strcmp0(name, hub->headset.def_sink_name) == 0);
		// g_signal_handlers_unblock_by_func(child,
		// 	G_CALLBACK(hub_power_toggled), hub);
		child = gtk_widget_get_next_sibling(child);
	}
}
