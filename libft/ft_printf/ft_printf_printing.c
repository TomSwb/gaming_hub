/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_printf_printing.c                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tschwab <tschwab@student.42lausanne.ch>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/29 17:54:13 by tschwab           #+#    #+#             */
/*   Updated: 2026/10/15 16:26:51 by tschwab          ###   ########.ch       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_printf.h"

/**
* @brief
Prints one character. Every output of ft_printf goes through here.
This is the only place calling write(): if it fails, the failure is reported to
error_state.

* @param char value
* @return int
*/

int	print_char(char value)
{
	int	error;

	error = write(1, &value, 1);
	error_state(error);
	return (1);
}

/**
* @brief
Prints a string, cut to the precision if there is one.
A NULL string is printed as "(null)", or not at all when the precision is under
6, like glibc.

* @param const char *value
* @param t_flags *flags
* @return int
*/

int	print_s(const char *value, t_flags *flags)
{
	int	i;
	int	count;

	i = 0;
	count = 0;
	if (value == NULL)
	{
		if (!flags->precision || flags->precision_len >= 6)
			value = "(null)";
		else
			return (count);
	}
	while (value[i] && (!flags->precision || i < flags->precision_len))
	{
		count += print_char(value[i]);
		i++;
	}
	return (count);
}

/**
* @brief
Prints a decimal number with its padding, sign and precision.
Order: sign first if zero padding, left padding, sign otherwise, precision
zeros, digits (from the highest power of 10 down), right padding.

* @param long long value
* @param int len
* @param t_flags *flags
* @return int
*/

int	print_deci(long long value, int len, t_flags *flags)
{
	int			count;
	long long	div;

	count = 0;
	div = def_div(value, flags);
	if (flags->zero_padding && !flags->precision && !flags->left_align
		&& (flags->converter == 'd' || flags->converter == 'i'))
		count += print_prefix(flags);
	if (!flags->left_align && flags->min_width > 0)
		count += print_padding(flags, len + pr_len(len, flags));
	if ((!flags->zero_padding || flags->precision || flags->left_align)
		&& (flags->converter == 'd' || flags->converter == 'i'))
		count += print_prefix(flags);
	if (flags->precision)
		count += print_precision(pr_len(len, flags));
	while (div > 0)
	{
		count += print_char((value / div) % 10 + 48);
		div /= 10;
	}
	if (flags->left_align && flags->min_width > 0)
		count += print_padding(flags, len + pr_len(len, flags));
	return (count);
}

/**
* @brief
Prints a hexadecimal number with its left padding, prefix and precision.
Order: left padding, 0x prefix (unless already printed for zero padding),
precision zeros, digits. Prints no digit for the value 0 with a precision of 0.
The right padding is printed by the manager.

* @param long long value
* @param char *base
* @param t_flags *flags
* @return int
*/

int	print_hexa(long long value, char *base, t_flags *flags)
{
	int			hex_len;
	int			count;
	long long	div;

	count = 0;
	hex_len = hexa_len(value, flags);
	div = def_div(value, flags);
	if (!flags->left_align && flags->min_width > 0)
		count += print_padding(flags, hex_len + pr_len(hex_len, flags));
	if ((!flags->zero_padding || flags->precision
			|| flags->left_align) && value != 0)
		count += print_prefix(flags);
	if (flags->precision && flags->precision_len != 0)
		count += print_precision(pr_len(hex_len, flags));
	if (value == 0 && flags->precision && flags->precision_len == 0)
		return (count);
	else
	{
		while (div > 0)
		{
			count += print_char(base[(value / div) % 16]);
			div /= 16;
		}
	}
	return (count);
}

/**
* @brief
Prints a non-NULL pointer address as 0x followed by lowercase hexadecimal.

* @param uintptr_t value
* @return int
*/

int	print_address(uintptr_t value)
{
	int			count;
	uintptr_t	div;
	char		*base;

	base = "0123456789abcdef";
	count = 0;
	count += print_char('0');
	count += print_char('x');
	div = 1;
	while (value / div >= 16)
	{
		if (div > value / 16)
			break ;
		div *= 16;
	}
	while (div > 0)
	{
		count += print_char(base[(value / div) % 16]);
		div /= 16;
	}
	return (count);
}
