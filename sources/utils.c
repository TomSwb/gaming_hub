
#include "gaming_hub.h"

void	hub_struct_init(t_hub *hub)
{
	hub->ram = NULL;
	hub->cpu = NULL;
	hub->prev_total = 0;
	hub->prev_idle = 0;
}
