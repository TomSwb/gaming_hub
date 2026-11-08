/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_memset.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tschwab <tschwab@student.42lausanne.ch>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/29 17:48:37 by tschwab           #+#    #+#             */
/*   Updated: 2026/10/15 16:44:08 by tschwab          ###   ########.ch       */
/*                                                                            */
/* ************************************************************************** */

/**
* @brief 
Returns the given 's' array filled with the 'c' char up to 'n' bytes.

* @param void *s 
* @param int c 
* @param size_t n 
* @return void* 
*/

#include "../libft.h"

void	*ft_memset(void *s, int c, size_t n)
{
	unsigned char	*u_s;
	unsigned char	u_c;

	u_c = (unsigned char)c;
	u_s = (unsigned char *)s;
	while (n > 0)
	{
		*u_s = u_c;
		u_s++;
		n--;
	}
	return (s);
}
