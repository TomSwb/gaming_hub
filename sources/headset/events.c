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

	(void)c;
	(void)user_data;
	facility = t & PA_SUBSCRIPTION_EVENT_FACILITY_MASK;
	g_print("pulse: event facility %u type %u idx %u\n", facility, t, idx);
}
