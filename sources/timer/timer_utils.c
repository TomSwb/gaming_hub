/* ************************************************************************** */
/*                                                                            */
/*                                            :::::::    :::     :::  :::::   */
/*   timer_utils.c                            :+:   :+:  :+:+: :+:+:   :+:    */
/*                                            +#++:+#+   +#+ +:+ +#+   +#+    */
/*   By: TomSwb <github.com/TomSwb>           +#+   +#+  +#+     +#+   +#+    */
/*                                            ########   ###     ###  #####   */
/*   Created: 2026/11/20 20:27:04 by TomSwb                                   */
/*   Updated: 2026/11/20 20:27:04 by TomSwb                   bitminded.ch    */
/*                                                                            */
/* ************************************************************************** */

#include "gaming_hub.h"

gboolean	hub_timer_running(t_hub *hub)
{
	if (hub->timer.state != TIMER_PLAYING && hub->timer.state != TIMER_BREAK
		&& hub->timer.state != TIMER_TRANSIT)
		return (FALSE);
	return (TRUE);
}
