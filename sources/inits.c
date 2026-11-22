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
	hub_setting_init(hub);
	hub_timer_init(hub);
	hub_headset_init(hub);
	hub_power_init(hub);
	hub->stack = NULL;
	hub->app = NULL;
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
	hub->computer.was_plugged = TRUE;
	hub->computer.low_warned = FALSE;
}

void	hub_setting_init(t_hub *hub)
{
	hub->setting.session = HUB_DEF_SESSION;
	hub->setting.wind_down = HUB_DEF_WIND_DOWN;
	hub->setting.break_min = HUB_DEF_BREAK_EVERY;
	hub->setting.eye_min = HUB_DEF_EYE_CUE_MIN;
	hub->setting.eye_sec = HUB_DEF_EYE_CUE_LENGTH;
	hub->setting.popups = HUB_DEF_POPUPS;
	hub->setting.sound = HUB_DEF_SOUND;
}

void	hub_timer_init(t_hub *hub)
{
	hub->timer.session = NULL;
	hub->timer.start = NULL;
	hub->timer.countdown = NULL;
	hub->timer.transit_countdown = NULL;
	hub->timer.pause = NULL;
	hub->timer.stop = NULL;
	hub->timer.transit_end = NULL;
	hub->timer.done = NULL;
	hub->timer.eye = NULL;
	hub->timer.break_spin = NULL;
	hub->timer.wind_spin = NULL;
	hub->timer.popups_check = NULL;
	hub->timer.sound_check = NULL;
	hub->timer.planned = 0;
	hub->timer.state = TIMER_PLANNING;
	hub->timer.started = 0;
	hub->timer.paused_at = 0;
	hub->timer.next_break = 0;
	hub->timer.next_eye = 0;
	hub->timer.eye_until = 0;
	hub->timer.wind_warned = FALSE;
	hub->timer.sound = NULL;
	hub->timer.popups = TRUE;
	hub->timer.sound_on = TRUE;
}

void	hub_headset_init(t_hub *hub)
{
	hub->headset.volume = NULL;
	hub->headset.sld_vol = NULL;
	hub->headset.mute_vol = NULL;
	hub->headset.mic = NULL;
	hub->headset.sld_mic = NULL;
	hub->headset.mute_mic = NULL;
	hub->headset.headset_data = NULL;
	hub->headset.adapter = NULL;
	hub->headset.context = NULL;
	hub->headset.sink_index = PA_INVALID_INDEX;
	hub->headset.src_index = PA_INVALID_INDEX;
	pa_cvolume_init(&hub->headset.sink_vol);
	pa_cvolume_init(&hub->headset.src_vol);
	hub->headset.moved_vol = 0;
	hub->headset.moved_mic = 0;
	hub->headset.obj_man = NULL;
	hub->headset.bt_add = NULL;
	hub->headset.outp_name = NULL;
	hub->headset.def_sink_name = NULL;
	hub->headset.def_src_name = NULL;
}
