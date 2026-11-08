/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_printf_parsing_utils.c                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tschwab <tschwab@student.42lausanne.ch>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/01 23:01:24 by tschwab           #+#    #+#             */
/*   Updated: 2026/10/15 16:15:56 by tschwab          ###   ########.ch       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_printf.h"

/**
* @brief
Checks if a character is one of the handled conversions.
Handled conversions: c s p d i u x X %.

* @param char c
* @return int
*/

int	is_converter(char c)
{
	char	*s;

	s = "cspdiuxX%";
	while (*s)
	{
		if (*s == c)
			return (1);
		s++;
	}
	return (0);
}

/**
* @brief
Checks if a character is a digit.

* @param char c
* @return int
*/

int	is_num(char c)
{
	if (c >= '0' && c <= '9')
		return (1);
	return (0);
}

/**
* @brief
Checks if a character is one of the handled flags.
Handled flags: - 0 + # and space.

* @param char c
* @return int
*/

int	is_flag(char c)
{
	char	*s;

	s = "-0+# ";
	while (*s)
	{
		if (*s == c)
			return (1);
		s++;
	}
	return (0);
}
