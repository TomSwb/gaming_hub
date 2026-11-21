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
	pa_operation		*sink_op;
	pa_operation		*source_op;

	state = pa_context_get_state(c);
	if (state == PA_CONTEXT_READY)
	{
		sink_op = pa_context_get_sink_info_list(c,
				hub_sink_list, user_data);
		if (sink_op)
			pa_operation_unref(sink_op);
		source_op = pa_context_get_source_info_list(c,
				hub_source_list, user_data);
		if (source_op)
			pa_operation_unref(source_op);
	}
	g_print("pulse: state %d\n", state);
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
		g_print("pulse: %s\n",
				pa_strerror(pa_context_errno(hub->headset.context)));
}
