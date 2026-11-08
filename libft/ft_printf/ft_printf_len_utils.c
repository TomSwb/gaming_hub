/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_printf_len_utils.c                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tschwab <tschwab@student.42lausanne.ch>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/30 02:55:43 by tschwab           #+#    #+#             */
/*   Updated: 2026/10/15 16:20:04 by tschwab          ###   ########.ch       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_printf.h"

/**
* @brief
Computes how many characters a string conversion will print.
A NULL string counts as "(null)" (6), or 0 when the precision is under 6, like
glibc. A precision can only shorten the string.

* @param char *s
* @param t_flags *flags
* @return int
*/

int	s_len(char *s, t_flags *flags)
{
	int	len;

	len = 0;
	if (s == NULL && flags->precision && flags->precision_len < 6)
		return (0);
	if (s == NULL)
		return (6);
	while (s[len])
		len++;
	if (flags->precision && len > flags->precision_len)
		len = flags->precision_len;
	return (len);
}

/**
* @brief
Computes how many characters a decimal number will take, sign included.
Counts the digits, plus one for a '-', or for a '+' or space on a positive d/i.
The value 0 counts as one digit. Precision zeros are not included, pr_len
computes those.

* @param long long value
* @param t_flags *flags
* @return int
*/

int	deci_len(long long value, t_flags *flags)
{
	int	len;

	len = 0;
	if (value >= 0 && ((flags->positive_sign || flags->space)
			&& flags->converter != 'u'))
		len++;
	if (value <= 0)
	{
		len++;
		value = -value;
	}
	while (value > 0)
	{
		len++;
		value /= 10;
	}
	return (len);
}

/**
* @brief
Computes how many zeros the precision adds in front of a number.
Removes the sign or the 0x prefix from the length first, since precision only
counts digits.

* @param int len
* @param t_flags *flags
* @return int
*/

int	pr_len(int len, t_flags *flags)
{
	int	pr_len;

	pr_len = 0;
	if (!flags->precision)
		return (0);
	pr_len = len;
	if (flags->neg_sign || ((flags->positive_sign || flags->space)
			&& flags->converter != 'u'))
		pr_len--;
	if (flags->alt_hexa && flags->precision_len != 0
		&& (flags->converter == 'x' || flags->converter == 'X'))
		pr_len -= 2;
	pr_len = flags->precision_len - pr_len;
	if (pr_len <= 0)
		pr_len = 0;
	return (pr_len);
}

/**
* @brief
Computes how many characters a hexadecimal number will take, prefix included.
Counts the hexadecimal digits, plus 2 for 0x/0X with the '#' flag. The value 0
is one digit, or nothing with a precision of 0.

* @param long long value
* @param t_flags *flags
* @return int
*/

int	hexa_len(long long value, t_flags *flags)
{
	int	len;

	len = 0;
	if (value == 0 && flags->precision
		&& flags->precision_len == 0)
		return (0);
	if (value == 0)
		return (1);
	if (flags->alt_hexa)
		len += 2;
	while (value > 0)
	{
		len++;
		value /= 16;
	}
	return (len);
}

/**
* @brief
Computes how many characters a pointer address will take.
Counts the hexadecimal digits plus 2 for the 0x prefix. A NULL pointer is
printed as "(nil)", so it counts 5.

* @param uintptr_t value
* @return int
*/

int	address_len(uintptr_t value)
{
	int	len;

	len = 2;
	if (value == 0)
		return (5);
	while (value > 0)
	{
		len++;
		value /= 16;
	}
	return (len);
}
