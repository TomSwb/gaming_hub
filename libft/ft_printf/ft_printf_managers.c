/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_printf_managers.c                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tschwab <tschwab@student.42lausanne.ch>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/01 22:58:34 by tschwab           #+#    #+#             */
/*   Updated: 2026/10/15 16:27:57 by tschwab          ###   ########.ch       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_printf.h"

/**
* @brief
Sends a parsed conversion to the right sub-manager.
Conversions are grouped by need: c/s/%, d/i/u, x/X and p.

* @param t_flags *flags
* @param va_list *args
* @param int *count
* @return void
*/

void	printer_manager(t_flags *flags, va_list *args, int *count)
{
	if (flags->converter == '%' || flags->converter == 'c'
		|| flags->converter == 's')
		printer_manager_char(flags, args, count);
	else if (flags->converter == 'd' || flags->converter == 'i'
		|| flags->converter == 'u')
		printer_manager_decimal(flags, args, count);
	else if (flags->converter == 'x' || flags->converter == 'X')
		printer_manager_hexa(flags, args, count);
	else if (flags->converter == 'p')
		printer_manager_address(flags, args, count);
}

/**
* @brief
Prints a %, c or s conversion with its padding.
Reads the argument for c (as an int, the promoted type) or s, then prints the
padding on the left or the right of it depending on '-'.

* @param t_flags *flags
* @param va_list *args
* @param int *count
* @return void
*/

void	printer_manager_char(t_flags *flags, va_list *args, int *count)
{
	char	c;
	char	*s;

	if (flags->converter == '%')
		*count += print_char('%');
	else if (flags->converter == 'c')
	{
		c = va_arg(*args, int);
		if (!flags->left_align && flags->min_width > 0)
			*count += print_padding(flags, 1);
		*count += print_char(c);
		if (flags->left_align && flags->min_width > 0)
			*count += print_padding(flags, 1);
	}
	else if (flags->converter == 's')
	{
		s = va_arg(*args, char *);
		if (!flags->left_align && flags->min_width > 0)
			*count += print_padding(flags, s_len(s, flags));
		*count += print_s(s, flags);
		if (flags->left_align && flags->min_width > 0)
			*count += print_padding(flags, s_len(s, flags));
	}
}

/**
* @brief
Prints a d, i or u conversion.
Reads the argument as an int or an unsigned int and stores it in a long long,
so INT_MIN can be made positive without overflow. Handles the value 0 with a
precision of 0 (no digit), and records a negative sign in neg_sign before
printing the absolute value.

* @param t_flags *flags
* @param va_list *args
* @param int *count
* @return void
*/

void	printer_manager_decimal(t_flags *flags, va_list *args, int *count)
{
	long long	value;
	int			len;

	if (flags->converter == 'u')
		value = (long long)va_arg(*args, unsigned int);
	else
		value = (long long)va_arg(*args, int);
	len = deci_len(value, flags);
	flags->neg_sign = 0;
	if (value == 0 && flags->precision && flags->precision_len == 0)
	{
		*count += print_nothing(len, flags);
		return ;
	}
	if (value < 0)
	{
		flags->neg_sign = 1;
		value = -value;
	}
	*count += print_deci(value, len, flags);
}

/**
* @brief
Prints an x or X conversion.
Picks the lowercase or uppercase base, turns '#' off for the value 0, prints
the 0x prefix first when zero padding is used, then the number and the right
padding.

* @param t_flags *flags
* @param va_list *args
* @param int *count
* @return void
*/

void	printer_manager_hexa(t_flags *flags, va_list *args, int *count)
{
	long long	value;
	char		*hexa_low;
	char		*hexa_up;
	char		*base;
	int			hex_len;

	base = NULL;
	value = (long long)va_arg(*args, unsigned int);
	hexa_low = "0123456789abcdef";
	hexa_up = "0123456789ABCDEF";
	if (flags->converter == 'X')
		base = hexa_up;
	else if (flags->converter == 'x')
		base = hexa_low;
	if (value == 0)
		flags->alt_hexa = 0;
	if ((flags->zero_padding && !flags->precision
			&& !flags->left_align) && value != 0)
		*count += print_prefix(flags);
	*count += print_hexa(value, base, flags);
	hex_len = hexa_len(value, flags);
	if (flags->left_align && flags->min_width > 0)
		*count += print_padding(flags, hex_len + pr_len(hex_len, flags));
}

/**
* @brief
Prints a p conversion with its padding.
A NULL pointer is printed as "(nil)", like glibc.

* @param t_flags *flags
* @param va_list *args
* @param int *count
* @return void
*/

void	printer_manager_address(t_flags *flags, va_list *args, int *count)
{
	void		*address;
	uintptr_t	value;
	int			adrs_len;

	address = va_arg(*args, void *);
	value = (uintptr_t)address;
	adrs_len = address_len(value);
	if (!flags->left_align && flags->min_width > 0)
		*count += print_padding(flags, adrs_len);
	if (value == 0)
		*count += print_s("(nil)", flags);
	else
		*count += print_address(value);
	if (flags->left_align && flags->min_width > 0)
		*count += print_padding(flags, adrs_len);
}
