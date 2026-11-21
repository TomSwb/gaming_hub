/* ************************************************************************** */
/*                                                                            */
/*                                            :::::::    :::     :::  :::::   */
/*   headset.h                                :+:   :+:  :+:+: :+:+:   :+:    */
/*                                            +#++:+#+   +#+ +:+ +#+   +#+    */
/*   By: TomSwb <github.com/TomSwb>           +#+   +#+  +#+     +#+   +#+    */
/*                                            ########   ###     ###  #####   */
/*   Created: 2026/11/21 14:22:14 by TomSwb                                   */
/*   Updated: 2026/11/21 14:22:14 by TomSwb                   bitminded.ch    */
/*                                                                            */
/* ************************************************************************** */

#ifndef HEADSET_H
# define HEADSET_H

// === Library === //

# include "structs.h"

// === Functions === //

// sources/headset/

// pulse.c
void		hub_pulse_state(pa_context *c, void *user_data);
void		hub_pulse_connect(t_hub *hub);

// sink.c
void		hub_sink_list(pa_context *c, const pa_sink_info *i, int eol,
				void *user_data);
void		hub_show_sink(t_hub *hub, const pa_sink_info *i);
void		hub_sink_set_buttons(t_hub *hub, gint per_vol,
				const pa_sink_info *i);
void		hub_sink_slider_moved(GtkRange *self, gpointer user_data);
void		hub_sink_mute_toggled(GtkToggleButton *self, gpointer user_data);

// source.c
void		hub_source_list(pa_context *c, const pa_source_info *i, int eol,
				void *user_data);
void		hub_show_source(t_hub *hub, const pa_source_info *i);
void		hub_source_set_buttons(t_hub *hub, gint per_mic,
				const pa_source_info *i);
void		hub_source_slider_moved(GtkRange *self, gpointer user_data);
void		hub_source_mute_toggled(GtkToggleButton *self, gpointer user_data);

// volume.c
int			hub_volume_percent(const pa_cvolume *volume);

// headset_build.c
GtkWidget	*hub_build_headset(t_hub *hub);
void		hub_build_sliders(t_hub *hub);
void		hub_build_mute(t_hub *hub);
void		hub_headset_build_box(t_hub *hub, GtkWidget *box, GtkWidget *label);

#endif
