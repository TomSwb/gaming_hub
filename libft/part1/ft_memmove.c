/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_memmove.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tschwab <tschwab@student.42lausanne.ch>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/30 11:04:35 by tschwab           #+#    #+#             */
/*   Updated: 2026/10/15 16:44:06 by tschwab          ###   ########.ch       */
/*                                                                            */
/* ************************************************************************** */

/**
* @brief 
Returns a pointer to the given 'dest' array after copying
'n' len of bytes from 'src' in it, ensuring overlap does not 
affect the data contrarily to memcpy which does not account for it.

* @param void dest 
* @param void const *src 
* @param size_t n 
* @return void* 
*/

#include "../libft.h"

void	*ft_memmove(void *dest, void const *src, size_t n)
{
	unsigned char		*u_dest;
	unsigned char		*u_src;
	size_t				i;

	u_dest = (unsigned char *)dest;
	u_src = (unsigned char *)src;
	i = 0;
	if (u_dest < u_src)
	{
		while (i < n)
		{
			u_dest[i] = u_src[i];
			i++;
		}
	}
	else
	{
		while (n > 0)
		{
			u_dest[n - 1] = u_src[n - 1];
			n--;
		}
	}
	return (dest);
}
