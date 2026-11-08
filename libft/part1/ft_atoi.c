/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_atoi.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tschwab <tschwab@student.42lausanne.ch>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/15 13:33:28 by tschwab           #+#    #+#             */
/*   Updated: 2026/10/15 16:43:41 by tschwab          ###   ########.ch       */
/*                                                                            */
/* ************************************************************************** */

/**
* @brief 
Converts the initial numerical portion of the given string to an int, 
skipping leading whitespace and accounting for a maximum of one + or - sign.

* @param char const *nptr
* @return int 
*/

#include "../libft.h"

int	ft_atoi(char const *nptr)
{
	int		result;
	size_t	i;
	int		sign;

	result = 0;
	i = 0;
	sign = 1;
	while (nptr[i] == 32
		|| (nptr[i] >= 9 && nptr[i] <= 13))
		i++;
	if (nptr[i] == '-' || nptr[i] == '+')
	{
		if (nptr[i] == '-')
			sign *= -sign;
		i++;
	}
	while (nptr[i] >= '0' && nptr[i] <= '9')
	{
		result = (result * 10) + (nptr[i] - '0');
		i++;
	}
	return (result * sign);
}
