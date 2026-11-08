/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strtrim.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tschwab <tschwab@student.42lausanne.ch>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/13 12:42:42 by tschwab           #+#    #+#             */
/*   Updated: 2026/10/15 16:45:56 by tschwab          ###   ########.ch       */
/*                                                                            */
/* ************************************************************************** */

/**
* @brief 
Returns a malloced char * containing s1 with all characters present 
in set removed from its beginning and end.

Depends on:
	ft_strdup();
		external -> malloc();

	ft_strchr();

	ft_strlen();

	ft_substr();
		ft_strlen();
		external -> malloc();

* @param char const *s1 
* @param char const *set 
* @return char* 
*/

#include "../libft.h"

char	*ft_strtrim(char const *s1, char const *set)
{
	size_t	end;
	size_t	start;
	size_t	len;

	if (!s1)
		return (NULL);
	if (!set)
		return (ft_strdup(s1));
	start = 0;
	while (s1[start] && ft_strchr(set, s1[start]) != NULL)
		start++;
	if (s1[start] == '\0')
		return (ft_strdup(s1 + start));
	end = ft_strlen(s1) - 1;
	while (end > start && ft_strchr(set, s1[end]) != NULL)
		end--;
	len = (end - start) + 1;
	return (ft_substr(s1, start, len));
}
