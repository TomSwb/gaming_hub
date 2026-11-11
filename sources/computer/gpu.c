/* ************************************************************************** */
/*                                                                            */
/*                                            :::::::    :::     :::  :::::   */
/*   gpu.c                                    :+:   :+:  :+:+: :+:+:   :+:    */
/*                                            +#++:+#+   +#+ +:+ +#+   +#+    */
/*   By: TomSwb <github.com/TomSwb>           +#+   +#+  +#+     +#+   +#+    */
/*                                            ########   ###     ###  #####   */
/*   Created: 2026/11/10 23:07:03 by TomSwb                                   */
/*   Updated: 2026/11/10 23:07:03 by TomSwb                   bitminded.ch    */
/*                                                                            */
/* ************************************************************************** */

#include "gaming_hub.h"

void	hub_update_gpu(t_hub *hub)
{
	gboolean	gpu_ok;
	guint64		gpu;
	gchar		*text;

	gpu = 0;
	gpu_ok = hub_read_number(hub->computer->gpu_dir, "gt_act_freq_mhz", &gpu);
	if (!gpu_ok)
	{
		gtk_label_set_text(GTK_LABEL(hub->computer->gpu), "GPU ... MHz");
		return ;
	}
	if (hub->computer->gpu_max == 0)
		text = g_strdup_printf("GPU %" G_GUINT64_FORMAT " / ... MHz", gpu);
	else
		text = g_strdup_printf("GPU %" G_GUINT64_FORMAT " / %"
				G_GUINT64_FORMAT " MHz", gpu, hub->computer->gpu_max);
	gtk_label_set_text(GTK_LABEL(hub->computer->gpu), text);
	g_free(text);
}
