/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_printf_parsing.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tschwab <tschwab@student.42lausanne.ch>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/29 17:54:07 by tschwab           #+#    #+#             */
/*   Updated: 2026/10/15 16:27:23 by tschwab          ###   ########.ch       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_printf.h"

/**
* @brief
Creates a t_flags with every field set to its "off" value.
Called for every % so nothing carries over from one conversion to the next.

* @return t_flags
*/

t_flags	init_flags(void)
{
	t_flags	flags;

	flags.left_align = 0;
	flags.zero_padding = 0;
	flags.positive_sign = 0;
	flags.alt_hexa = 0;
	flags.space = 0;
	flags.neg_sign = 0;
	flags.min_width = 0;
	flags.precision = 0;
	flags.precision_len = 0;
	flags.converter = '\0';
	return (flags);
}

/**
* @brief
Reads one conversion of the format string into the struct.
Skips the %, reads the flags (-0+# and space), then the width and the
precision, then the converter. If the converter is valid, flag conflicts are
resolved by parsing_check.

* @param const char **s
* @param t_flags *flags
* @return int
*/

int	parser(const char **s, t_flags *flags)
{
	(*s)++;
	while (**s && is_flag(**s))
	{
		if (**s == '-')
			flags->left_align = 1;
		else if (**s == '0')
			flags->zero_padding = 1;
		else if (**s == '+')
			flags->positive_sign = 1;
		else if (**s == '#')
			flags->alt_hexa = 1;
		else if (**s == ' ')
			flags->space = 1;
		(*s)++;
	}
	parse_min_width_precison(flags, s);
	flags->converter = **s;
	if (!is_converter(flags->converter))
		return (0);
	(*s)++;
	parsing_check(flags);
	return (1);
}

/**
* @brief
Reads the minimum width and the precision of a conversion.
Digits right after the flags are the width. A '.' turns precision on, and the
digits after it are the precision length (none means 0).

* @param t_flags *flags
* @param const char **s
* @return void
*/

void	parse_min_width_precison(t_flags *flags, const char **s)
{
	if (is_num(**s))
	{
		while (is_num(**s))
		{
			flags->min_width = flags->min_width * 10 + (**s - 48);
			(*s)++;
		}
	}
	if (**s == '.')
	{
		flags->precision = 1;
		(*s)++;
		while (is_num(**s))
		{
			flags->precision_len = flags->precision_len * 10 + (**s - 48);
			(*s)++;
		}
	}
}

/**
* @brief
Resolves flag conflicts so the printers never see a contradiction.
Never rejects a format: the losing flag is switched off, following printf's
rules. '-' beats '0', a precision beats '0' (except on s), '+' beats space. '+'
and space only apply to d/i, '#' only to x/X, '0' does not apply to c/s/p and
precision does not apply to c/p.

* @param t_flags *flags
* @return void
*/

void	parsing_check(t_flags *flags)
{
	if (flags->left_align)
		flags->zero_padding = 0;
	if (flags->precision && flags->converter != 's')
		flags->zero_padding = 0;
	if (flags->positive_sign)
		flags->space = 0;
	if (flags->converter != 'd' && flags->converter != 'i')
	{
		flags->positive_sign = 0;
		flags->space = 0;
	}
	if (flags->converter != 'x' && flags->converter != 'X')
		flags->alt_hexa = 0;
	if (flags->converter == 'c' || flags->converter == 's'
		|| flags->converter == 'p')
		flags->zero_padding = 0;
	if (flags->converter == 'c' || flags->converter == 'p')
	{
		flags->precision = 0;
		flags->precision_len = 0;
	}
}
