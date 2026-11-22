/* ************************************************************************** */
/*                                                                            */
/*                                            :::::::    :::     :::  :::::   */
/*   events.c                                 :+:   :+:  :+:+: :+:+:   :+:    */
/*                                            +#++:+#+   +#+ +:+ +#+   +#+    */
/*   By: TomSwb <github.com/TomSwb>           +#+   +#+  +#+     +#+   +#+    */
/*                                            ########   ###     ###  #####   */
/*   Created: 2026/11/21 23:44:04 by TomSwb                                   */
/*   Updated: 2026/11/21 23:44:04 by TomSwb                   bitminded.ch    */
/*                                                                            */
/* ************************************************************************** */

#include "gaming_hub.h"

void	hub_pulse_event(pa_context *c, pa_subscription_event_type_t t,
			uint32_t idx, void *user_data)
{
	pa_subscription_event_type_t	facility;
	pa_subscription_event_type_t	type;
	pa_operation					*op;

	facility = t & PA_SUBSCRIPTION_EVENT_FACILITY_MASK;
	type = t & PA_SUBSCRIPTION_EVENT_TYPE_MASK;
	if (facility == PA_SUBSCRIPTION_EVENT_SINK)
		hub_sink_event(user_data, c, type, idx);
	else if (facility == PA_SUBSCRIPTION_EVENT_SOURCE)
		hub_source_event(user_data, c, type, idx);
	else if (facility == PA_SUBSCRIPTION_EVENT_SERVER)
	{
		op = pa_context_get_server_info(c, hub_pulse_default, user_data);
		if (op)
			pa_operation_unref(op);
	}
}

void	hub_sink_event(t_hub *hub, pa_context *c,
			pa_subscription_event_type_t type, uint32_t idx)
{
	pa_operation	*op;

	if (type == PA_SUBSCRIPTION_EVENT_REMOVE && idx == hub->headset.sink_index)
	{
		hub->headset.sink_index = PA_INVALID_INDEX;
		hub_show_sink(hub, NULL);
	}
	else if (type == PA_SUBSCRIPTION_EVENT_NEW
		|| (type == PA_SUBSCRIPTION_EVENT_CHANGE
			&& idx == hub->headset.sink_index))
	{
		op = pa_context_get_sink_info_by_index(c, idx,
				hub_sink_list, hub);
		if (op)
			pa_operation_unref(op);
	}
}

void	hub_source_event(t_hub *hub, pa_context *c,
			pa_subscription_event_type_t type, uint32_t idx)
{
	pa_operation	*op;

	if (type == PA_SUBSCRIPTION_EVENT_REMOVE && idx == hub->headset.src_index)
	{
		hub->headset.src_index = PA_INVALID_INDEX;
		hub_show_source(hub, NULL);
	}
	else if (type == PA_SUBSCRIPTION_EVENT_NEW
		|| (type == PA_SUBSCRIPTION_EVENT_CHANGE
			&& idx == hub->headset.src_index))
	{
		op = pa_context_get_source_info_by_index(c, idx,
				hub_source_list, hub);
		if (op)
			pa_operation_unref(op);
	}
}
