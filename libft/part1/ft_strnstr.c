/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strnstr.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tschwab <tschwab@student.42lausanne.ch>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/25 17:57:24 by tschwab           #+#    #+#             */
/*   Updated: 2026/10/15 16:44:28 by tschwab          ###   ########.ch       */
/*                                                                            */
/* ************************************************************************** */

/**
 * @brief 
Locates the first occurrence of the 
null-terminated string 'little' in the string 'big', 
where not more than 'len' characters are searched. 
Characters that appear after a '\0' character are not searched. 

* @param char const *big 
* @param char const *little 
* @param size_t len 
* @return char* 
*/

#include "../libft.h"

char	*ft_strnstr(char const *big, char const *little, size_t len)
{
	size_t	i;
	size_t	j;

	if (little[0] == '\0')
		return ((char *)big);
	i = 0;
	while (i < len && big[i])
	{
		j = 0;
		while ((i + j) < len && little[j] && big[i + j] == little[j])
			j++;
		if (little[j] == '\0')
			return ((char *)&big[i]);
		i++;
	}
	return (NULL);
}
