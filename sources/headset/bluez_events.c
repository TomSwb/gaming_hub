/* ************************************************************************** */
/*                                                                            */
/*                                            :::::::    :::     :::  :::::   */
/*   bluez_events.c                           :+:   :+:  :+:+: :+:+:   :+:    */
/*                                            +#++:+#+   +#+ +:+ +#+   +#+    */
/*   By: TomSwb <github.com/TomSwb>           +#+   +#+  +#+     +#+   +#+    */
/*                                            ########   ###     ###  #####   */
/*   Created: 2026/11/22 18:20:33 by TomSwb                                   */
/*   Updated: 2026/11/22 18:20:33 by TomSwb                   bitminded.ch    */
/*                                                                            */
/* ************************************************************************** */

#include "gaming_hub.h"

void	hub_bluez_iface(GDBusObjectManager *manager, GDBusObject *object,
			GDBusInterface *interface, gpointer user_data)
{
	(void)manager;
	(void)object;
	(void)interface;
	hub_bluez_show(user_data);
}

void	hub_bluez_props(GDBusObjectManagerClient *manager,
			GDBusObjectProxy *object_proxy, GDBusProxy *interface_proxy,
			GVariant *changed, GStrv invalidated, gpointer user_data)
{
	(void)manager;
	(void)object_proxy;
	(void)interface_proxy;
	(void)changed;
	(void)invalidated;
	hub_bluez_show(user_data);
}
