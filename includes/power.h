/* ************************************************************************** */
/*                                                                            */
/*                                            :::::::    :::     :::  :::::   */
/*   power.h                                  :+:   :+:  :+:+: :+:+:   :+:    */
/*                                            +#++:+#+   +#+ +:+ +#+   +#+    */
/*   By: TomSwb <github.com/TomSwb>           +#+   +#+  +#+     +#+   +#+    */
/*                                            ########   ###     ###  #####   */
/*   Created: 2026/11/22 09:46:53 by TomSwb                                   */
/*   Updated: 2026/11/22 09:46:53 by TomSwb                   bitminded.ch    */
/*                                                                            */
/* ************************************************************************** */

#ifndef POWER_H
# define POWER_H

// === Library === //

# include "structs.h"

// === Functions === //

// sources/power/

// power.c
void	hub_power_connect(t_hub *hub);
void	hub_power_ready(GObject *source, GAsyncResult *res, gpointer user_data);

#endif
