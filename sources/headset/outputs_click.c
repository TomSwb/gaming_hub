/* ************************************************************************** */
/*                                                                            */
/*                                            :::::::    :::     :::  :::::   */
/*   outputs_click.c                          :+:   :+:  :+:+: :+:+:   :+:    */
/*                                            +#++:+#+   +#+ +:+ +#+   +#+    */
/*   By: TomSwb <github.com/TomSwb>           +#+   +#+  +#+     +#+   +#+    */
/*                                            ########   ###     ###  #####   */
/*   Created: 2026/11/22 22:37:07 by TomSwb                                   */
/*   Updated: 2026/11/22 22:37:07 by TomSwb                   bitminded.ch    */
/*                                                                            */
/* ************************************************************************** */

#include "gaming_hub.h"

void	hub_outputs_toggled(GtkToggleButton *self, gpointer user_data)
{
	const gchar		*name;
	t_hub			*hub;
	pa_operation	*op;

	hub = user_data;
	if (gtk_toggle_button_get_active(self))
	{
		name = g_object_get_data(G_OBJECT(self), HUB_PA_KEY);
		op = pa_context_set_default_sink(hub->headset.context, name,
				NULL, NULL);
		if (op)
			pa_operation_unref(op);
	}
}
