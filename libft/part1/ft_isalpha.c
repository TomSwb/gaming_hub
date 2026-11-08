/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_isalpha.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tschwab <tschwab@student.42lausanne.ch>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/03 16:42:15 by tschwab           #+#    #+#             */
/*   Updated: 2026/10/12 19:03:30 by tschwab          ###   ########.ch       */
/*                                                                            */
/* ************************************************************************** */

/**
* @brief 
checks for an alphabetic character; in the standard  "C"
locale,  it is equivalent to (isupper(c) || islower(c)).
In some locales, there may be additional characters  for
which isalpha() is true-letters which are neither upper-
case nor lowercase.

* @param int c 
* @return int 
*/

int	ft_isalpha(int c)
{
	if ((c >= 'A' && c <= 'Z')
		|| (c >= 'a' && c <= 'z'))
		return (1);
	return (0);
}
