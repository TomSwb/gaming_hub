/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_tolower.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tschwab <tschwab@student.42lausanne.ch>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/03 17:34:09 by tschwab           #+#    #+#             */
/*   Updated: 2026/08/29 16:36:23 by tschwab          ###   ########.ch       */
/*                                                                            */
/* ************************************************************************** */

/**
* @brief 
If c is an uppercase letter, tolower() returns its lowercase equivalent, 
if a lowercase representation exists in  the  current locale.  
Otherwise, it returns c.
* 
* @param int c 
* @return int 
*/

int	ft_tolower(int c)
{
	if (c >= 'A' && c <= 'Z')
		return (c + 32);
	return (c);
}
