/* ************************************************************************** */
/*                                                                            */
/*                                            :::::::    :::     :::  :::::   */
/*   inits.c                                  :+:   :+:  :+:+: :+:+:   :+:    */
/*                                            +#++:+#+   +#+ +:+ +#+   +#+    */
/*   By: TomSwb <github.com/TomSwb>           +#+   +#+  +#+     +#+   +#+    */
/*                                            ########   ###     ###  #####   */
/*   Created: 2026/11/11 14:00:14 by TomSwb                                   */
/*   Updated: 2026/11/11 14:00:14 by TomSwb                   bitminded.ch    */
/*                                                                            */
/* ************************************************************************** */

#include "gaming_hub.h"

void	hub_struct_init(t_hub *hub)
{
	hub_computer_init(hub);
	hub_timer_init(hub);
}

void	hub_computer_init(t_hub *hub)
{
	hub->computer.ram = NULL;
	hub->computer.cpu = NULL;
	hub->computer.temp = NULL;
	hub->computer.gpu = NULL;
	hub->computer.bat = NULL;
	hub->computer.prev_total = 0;
	hub->computer.prev_idle = 0;
	hub->computer.temp_dir = NULL;
	hub->computer.fan_dir = NULL;
	hub->computer.gpu_dir = NULL;
	hub->computer.gpu_max = 0;
	hub->computer.bat_dir = NULL;
	hub->computer.mains_dir = NULL;
}

void	hub_timer_init(t_hub *hub)
{
	hub->setting.session = HUB_DEF_SESSION;
	hub->setting.wind_down = HUB_DEF_WIND_DOWN;
	hub->setting.break_min = HUB_DEF_BREAK_MIN;
	hub->setting.eye_min = HUB_DEF_EYE_CUE_MIN;
	hub->setting.eye_sec = HUB_DEF_EYE_CUE_LENGTH;
	hub->timer.session = NULL;
	hub->timer.start = NULL;
}
