/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_printf_printing_utils.c                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tschwab <tschwab@student.42lausanne.ch>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/29 17:51:30 by tschwab           #+#    #+#             */
/*   Updated: 2026/10/25 16:16:53 by tschwab          ###   ########.ch       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_printf.h"

/**
* @brief
Prints the spaces or zeros needed to reach the minimum width.
Zeros only for numbers with the '0' flag, without '-' and without a precision;
spaces otherwise.

* @param t_flags *flags
* @param int len_value
* @return int
*/

int	print_padding(t_flags *flags, int len_value)
{
	int	count;
	int	i;

	count = flags->min_width - len_value;
	if (count <= 0)
		return (0);
	i = 0;
	while (i < count)
	{
		if (flags->zero_padding && !flags->left_align && !flags->precision
			&& flags->converter != 'c' && flags->converter != 's'
			&& flags->converter != 'p')
			print_char('0');
		else
			print_char(' ');
		i++;
	}
	return (count);
}

/**
* @brief
Prints what comes before the digits of a number.
Only one of: '-' for a negative number, '+', a space, or 0x/0X for '#' on a
hexadecimal number.

* @param t_flags *flags
* @return int
*/

int	print_prefix(t_flags *flags)
{
	int	count;

	count = 0;
	if (flags->neg_sign)
		count += print_char('-');
	else if (flags->positive_sign)
		count += print_char('+');
	else if (flags->space)
		count += print_char(' ');
	else if (flags->alt_hexa)
	{
		count += print_char('0');
		if (flags->converter == 'x')
			count += print_char('x');
		else if (flags->converter == 'X')
			count += print_char('X');
	}
	return (count);
}

/**
* @brief
Prints the zeros added by the precision.

* @param int pr_len
* @return int
*/

int	print_precision(int pr_len)
{
	int	count;

	count = 0;
	while (pr_len > 0)
	{
		count += print_char('0');
		pr_len--;
	}
	return (count);
}

/**
* @brief
Prints a number of value 0 with a precision of 0: no digit.
Only the padding and, for d/i, a '+' or space prefix are printed.

* @param int len
* @param t_flags *flags
* @return int
*/

int	print_nothing(int len, t_flags *flags)
{
	int	count;

	count = 0;
	if (!flags->left_align)
		count += print_padding(flags, len - 1);
	if (flags->converter == 'd' || flags->converter == 'i')
		count += print_prefix(flags);
	if (flags->left_align)
		count += print_padding(flags, len - 1);
	return (count);
}

/**
* @brief
Finds the highest power of the base that fits in the value.
The printers divide by it to print the digits from left to right, then divide
it by the base after each digit. Base 10 for d/i/u, base 16 for x/X.

* @param long long value
* @param t_flags *flags
* @return long long
*/

long long	def_div(long long value, t_flags *flags)
{
	long long	div;
	int			base;

	base = 0;
	if (flags->converter == 'd' || flags->converter == 'i'
		|| flags->converter == 'u')
		base = 10;
	else if (flags->converter == 'x' || flags->converter == 'X')
		base = 16;
	div = 1;
	while (value / div >= base)
	{
		if (div > value / base)
			break ;
		div *= base;
	}
	return (div);
}
