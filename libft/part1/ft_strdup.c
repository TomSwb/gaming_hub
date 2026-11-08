/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strdup.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tschwab <tschwab@student.42lausanne.ch>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/30 20:44:54 by tschwab           #+#    #+#             */
/*   Updated: 2026/10/15 16:43:33 by tschwab          ###   ########.ch       */
/*                                                                            */
/* ************************************************************************** */

/**
* @brief 
Returns an array with the original 's' string duplicated in it (copied) 
but in the heap instead of the stack, using malloc.

External:
	malloc();
	
* @param char const *s 
* @return char* 
*/

#include "../libft.h"

char	*ft_strdup(char const *s)
{
	size_t	len;
	char	*dest;
	size_t	i;

	if (!s)
		return (NULL);
	len = 0;
	while (s[len])
		len++;
	dest = malloc(len + 1);
	if (!dest)
		return (NULL);
	i = 0;
	while (i < len)
	{
		dest[i] = s[i];
		i++;
	}
	dest[i] = '\0';
	return (dest);
}
