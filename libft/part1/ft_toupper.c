/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_toupper.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tschwab <tschwab@student.42lausanne.ch>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/03 17:20:38 by tschwab           #+#    #+#             */
/*   Updated: 2026/08/29 16:36:25 by tschwab          ###   ########.ch       */
/*                                                                            */
/* ************************************************************************** */

/**
* @brief 
If c is a lowercase letter, toupper() returns  its  uppercase  equivalent, 
if an uppercase representation exists in the current locale.  
Otherwise, it  returns  c.
* 
* @param int c 
* @return int 
*/

int	ft_toupper(int c)
{
	if (c >= 'a' && c <= 'z')
		return (c - 32);
	return (c);
}
