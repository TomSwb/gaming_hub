/* ************************************************************************** */
/*                                                                            */
/*                                            :::::::    :::     :::  :::::   */
/*   pulse.c                                  :+:   :+:  :+:+: :+:+:   :+:    */
/*                                            +#++:+#+   +#+ +:+ +#+   +#+    */
/*   By: TomSwb <github.com/TomSwb>           +#+   +#+  +#+     +#+   +#+    */
/*                                            ########   ###     ###  #####   */
/*   Created: 2026/11/21 14:28:11 by TomSwb                                   */
/*   Updated: 2026/11/21 14:28:11 by TomSwb                   bitminded.ch    */
/*                                                                            */
/* ************************************************************************** */

#include "gaming_hub.h"

void	hub_pulse_state(pa_context *c, void *user_data)
{
	pa_context_state_t	state;
	t_hub				*hub;

	hub = user_data;
	state = pa_context_get_state(c);
	if (state == PA_CONTEXT_READY)
		hub_pulse_ready(c, user_data);
	else if (state == PA_CONTEXT_FAILED)
		g_printerr("pulse: states: %d, %s\n", state,
			pa_strerror(pa_context_errno(hub->headset.context)));
}

void	hub_pulse_connect(t_hub *hub)
{
	pa_mainloop_api	*api;

	hub->headset.adapter = pa_glib_mainloop_new(NULL);
	if (!hub->headset.adapter)
		return ;
	api = pa_glib_mainloop_get_api(hub->headset.adapter);
	hub->headset.context = pa_context_new(api, "Gaming Hub");
	if (!hub->headset.context)
		return ;
	pa_context_set_state_callback(hub->headset.context, hub_pulse_state, hub);
	if (pa_context_connect(hub->headset.context, NULL,
			PA_CONTEXT_NOFLAGS, NULL) < 0)
		g_printerr("pulse: %s\n",
			pa_strerror(pa_context_errno(hub->headset.context)));
}

void	hub_pulse_ready(pa_context *c, void *user_data)
{
	pa_operation	*op;

	op = pa_context_get_sink_info_list(c,
			hub_sink_list, user_data);
	if (op)
		pa_operation_unref(op);
	op = pa_context_get_source_info_list(c,
			hub_source_list, user_data);
	if (op)
		pa_operation_unref(op);
	pa_context_set_subscribe_callback(c, hub_pulse_event, user_data);
	op = pa_context_subscribe(c,
			PA_SUBSCRIPTION_MASK_SINK | PA_SUBSCRIPTION_MASK_SOURCE,
			NULL, NULL);
	if (op)
		pa_operation_unref(op);
}
