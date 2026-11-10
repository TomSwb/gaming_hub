
#include "gaming_hub.h"

void	hub_update_gpu(t_hub *hub)
{
	gchar	*gpu;
	gchar	*text;

	gpu = hub_find_gpu();
	if (!gpu)
		return ;
	text = g_strdup_printf("GPU %s Mhz", gpu);
	gtk_label_set_text(GTK_LABEL(hub->gpu), text);
	g_free(gpu);
	g_free(text);
}
