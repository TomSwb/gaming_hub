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
void		hub_pulse_ready(pa_context *c, void *user_data);

// events.c
void		hub_pulse_event(pa_context *c, pa_subscription_event_type_t t,
				uint32_t idx, void *user_data);
void		hub_sink_event(t_hub *hub, pa_context *c,
				pa_subscription_event_type_t type, uint32_t idx);
void		hub_source_event(t_hub *hub, pa_context *c,
				pa_subscription_event_type_t type, uint32_t idx);

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

// default.c
void		hub_pulse_default(pa_context *c, const pa_server_info *i,
				void *user_data);
void		hub_default_sink(pa_context *c, const pa_sink_info *i, int eol,
				void *user_data);
void		hub_set_output(t_hub *hub, const char *address, const char *name);
void		hub_ask_default_sink(t_hub *hub, const char *sink);
void		hub_ask_default_source(t_hub *hub, const char *source);

// volume.c
int			hub_volume_percent(const pa_cvolume *volume);

// bluez.c
void		hub_bluez_connect(t_hub *hub);
void		hub_bluez_ready(GObject *source, GAsyncResult *res,
				gpointer user_data);
gboolean	hub_bluez_has_address(GDBusObject *object, const gchar *address);
GDBusObject	*hub_bluez_find(t_hub *hub);

// bluez_show.c
GVariant	*hub_bluez_prop(GDBusObject *object, const gchar *iface,
				const gchar *prop);
void		hub_bluez_show(t_hub *hub);
gchar		*hub_bluez_text(GDBusObject *device);
gchar		*hub_bluez_with_battery(GDBusObject *device, const gchar *name);

// headset_build.c
GtkWidget	*hub_build_headset(t_hub *hub);
void		hub_build_sliders(t_hub *hub);
void		hub_build_mute(t_hub *hub);
void		hub_headset_build_box(t_hub *hub, GtkWidget *box, GtkWidget *label);

#endif
