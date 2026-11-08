/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_memcpy.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tschwab <tschwab@student.42lausanne.ch>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/29 17:12:42 by tschwab           #+#    #+#             */
/*   Updated: 2026/10/15 16:44:04 by tschwab          ###   ########.ch       */
/*                                                                            */
/* ************************************************************************** */

/**
* @brief 
Returns a pointer to the given 'dest' array after copying 'n' bytes 
values from 'src'.

* @param void dest 
* @param void const *src 
* @param size_t n 
* @return void* 
*/

#include "../libft.h"

void	*ft_memcpy(void *dest, void const *src, size_t n)
{
	unsigned char		*u_dest;
	const unsigned char	*u_src;

	u_dest = (unsigned char *)dest;
	u_src = (const unsigned char *)src;
	while (n > 0)
	{
		*u_dest = *u_src;
		u_dest++;
		u_src++;
		n--;
	}
	return (dest);
}
