/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strjoin.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tschwab <tschwab@student.42lausanne.ch>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/02 16:44:25 by tschwab           #+#    #+#             */
/*   Updated: 2026/10/15 16:45:50 by tschwab          ###   ########.ch       */
/*                                                                            */
/* ************************************************************************** */

/**
* @brief 
Returns a 'char *' containing the concatenation of s1 and s2 
in allocated memory.

Depends on:
	ft_strlen();
	ft_strlcpy(); Note: ft_strcpy would be sufficient.
	ft_strlcat(); Note: ft_strcat would be sufficient.

External:
	malloc();

* @param char const *s1 
* @param char const *s2 
* @return char* 
*/

#include "../libft.h"

char	*ft_strjoin(char const *s1, char const *s2)
{
	char	*result;
	size_t	s_s1;
	size_t	s_s2;
	size_t	size;

	if (!s1 || !s2)
		return (NULL);
	s_s1 = ft_strlen(s1);
	s_s2 = ft_strlen(s2);
	size = s_s1 + s_s2 + 1;
	result = malloc(size);
	if (!result)
		return (NULL);
	ft_strlcpy(result, s1, size);
	ft_strlcat(result, s2, size);
	return (result);
}
