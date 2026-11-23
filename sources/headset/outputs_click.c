/* ************************************************************************** */
/*                                                                            */
/*                                            :::::::    :::     :::  :::::   */
/*   outputs_click.c                          :+:   :+:  :+:+: :+:+:   :+:    */
/*                                            +#++:+#+   +#+ +:+ +#+   +#+    */
/*   By: TomSwb <github.com/TomSwb>           +#+   +#+  +#+     +#+   +#+    */
/*                                            ########   ###     ###  #####   */
/*   Created: 2026/11/22 22:37:07 by TomSwb                                   */
/*   Updated: 2026/11/22 22:37:07 by TomSwb                   bitminded.ch    */
/*                                                                            */
/* ************************************************************************** */

#include "gaming_hub.h"

void	hub_outputs_toggled(GtkToggleButton *self, gpointer user_data)
{
	const gchar		*name;
	t_hub			*hub;
	pa_operation	*op;

	hub = user_data;
	if (gtk_toggle_button_get_active(self))
	{
		name = g_object_get_data(G_OBJECT(self), HUB_PA_KEY);
		op = pa_context_set_default_sink(hub->headset.context, name,
				NULL, NULL);
		if (op)
			pa_operation_unref(op);
		g_free(hub->headset.want_add);
		hub->headset.want_add = g_strdup(g_object_get_data(G_OBJECT(self),
					HUB_PA_ADDR));
		op = pa_context_get_source_info_list(hub->headset.context,
				hub_mic_scan, hub);
		if (op)
			pa_operation_unref(op);
	}
}

void	hub_mic_scan(pa_context *c, const pa_source_info *i, int eol,
			void *user_data)
{
	t_hub		*hub;
	const char	*address;

	(void)c;
	hub = user_data;
	if (eol > 0)
	{
		hub_mic_apply(hub);
		return ;
	}
	else if (eol < 0 || !i)
		return ;
	else if (i->monitor_of_sink != PA_INVALID_INDEX)
		return ;
	else if (i->active_port
		&& i->active_port->available == PA_PORT_AVAILABLE_NO)
		return ;
	address = pa_proplist_gets(i->proplist, HUB_PA_BT_ADDRESS);
	if (hub->headset.want_add && g_strcmp0(hub->headset.want_add, address) == 0)
		hub_keep(&hub->headset.head_mic, i->name);
	else if (!address && !hub->headset.lapt_mic)
		hub_keep(&hub->headset.lapt_mic, i->name);
}

void	hub_mic_apply(t_hub *hub)
{
	const gchar		*choice;
	pa_operation	*op;

	if (hub->headset.head_mic)
		choice = hub->headset.head_mic;
	else
		choice = hub->headset.lapt_mic;
	if (choice)
	{
		op = pa_context_set_default_source(hub->headset.context, choice,
				NULL, NULL);
		if (op)
			pa_operation_unref(op);
	}
	hub_keep(&hub->headset.head_mic, NULL);
	hub_keep(&hub->headset.lapt_mic, NULL);
}
