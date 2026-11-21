/* ************************************************************************** */
/*                                                                            */
/*                                            :::::::    :::     :::  :::::   */
/*   volume.c                                 :+:   :+:  :+:+: :+:+:   :+:    */
/*                                            +#++:+#+   +#+ +:+ +#+   +#+    */
/*   By: TomSwb <github.com/TomSwb>           +#+   +#+  +#+     +#+   +#+    */
/*                                            ########   ###     ###  #####   */
/*   Created: 2026/11/21 17:46:09 by TomSwb                                   */
/*   Updated: 2026/11/21 17:46:09 by TomSwb                   bitminded.ch    */
/*                                                                            */
/* ************************************************************************** */

#include "gaming_hub.h"

int	hub_volume_percent(const pa_cvolume *volume)
{
	pa_volume_t	channels;
	int			result;

	channels = pa_cvolume_avg(volume);
	result = ((channels * 100) + (PA_VOLUME_NORM / 2)) / PA_VOLUME_NORM;
	return (result);
}
