/* ************************************************************************** */
/*                                                                            */
/*                                            :::::::    :::     :::  :::::   */
/*   default.c                                :+:   :+:  :+:+: :+:+:   :+:    */
/*                                            +#++:+#+   +#+ +:+ +#+   +#+    */
/*   By: TomSwb <github.com/TomSwb>           +#+   +#+  +#+     +#+   +#+    */
/*                                            ########   ###     ###  #####   */
/*   Created: 2026/11/22 18:57:51 by TomSwb                                   */
/*   Updated: 2026/11/22 18:57:51 by TomSwb                   bitminded.ch    */
/*                                                                            */
/* ************************************************************************** */

#include "gaming_hub.h"

void	hub_pulse_default(pa_context *c, const pa_server_info *i,
		void *user_data)
{
	pa_operation	*op;

	if (!i)
	{
		hub_ask_default_sink(user_data, NULL);
		hub_ask_default_source(user_data, NULL);
	}
	else
	{
		hub_ask_default_sink(user_data, i->default_sink_name);
		hub_ask_default_source(user_data, i->default_source_name);
	}
	if (!i || !i->default_sink_name)
		hub_set_output(user_data, NULL, NULL);
	else
	{
		op = pa_context_get_sink_info_by_name(c, i->default_sink_name,
				hub_default_sink, user_data);
		if (op)
			pa_operation_unref(op);
	}
}

void	hub_default_sink(pa_context *c, const pa_sink_info *i, int eol,
			void *user_data)
{
	const gchar	*address;
	const gchar	*name;

	(void)c;
	if (eol > 0 || !i)
		return ;
	address = pa_proplist_gets(i->proplist, HUB_PA_BT_ADDRESS);
	name = pa_proplist_gets(i->proplist, HUB_PA_SHORT_NAME);
	if (!name)
		name = i->description;
	hub_set_output(user_data, address, name);
}

void	hub_set_output(t_hub *hub, const char *address, const char *name)
{
	g_free(hub->headset.bt_add);
	g_free(hub->headset.outp_name);
	hub->headset.bt_add = g_strdup(address);
	hub->headset.outp_name = g_strdup(name);
	hub_bluez_show(hub);
}

void	hub_ask_default_sink(t_hub *hub, const char *sink)
{
	pa_operation	*op;

	g_free(hub->headset.def_sink_name);
	hub->headset.def_sink_name = g_strdup(sink);
	if (hub->headset.def_sink_name)
	{
		op = pa_context_get_sink_info_by_name(hub->headset.context,
			hub->headset.def_sink_name, hub_sink_list, hub);
		if (op)
			pa_operation_unref(op);
	}
	else
	{
		hub->headset.sink_index = PA_INVALID_INDEX;
		hub_show_sink(hub, NULL);
	}
}

void	hub_ask_default_source(t_hub *hub, const char *source)
{
	pa_operation	*op;

	g_free(hub->headset.def_src_name);
	hub->headset.def_src_name = g_strdup(source);
	if (hub->headset.def_src_name)
	{
		op = pa_context_get_source_info_by_name(hub->headset.context,
			hub->headset.def_src_name, hub_source_list, hub);
		if (op)
			pa_operation_unref(op);
	}
	else
	{
		hub->headset.src_index = PA_INVALID_INDEX;
		hub_show_source(hub, NULL);
	}
}
