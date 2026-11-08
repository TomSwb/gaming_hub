/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_printf.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tschwab <tschwab@student.42lausanne.ch>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/29 17:51:26 by tschwab           #+#    #+#             */
/*   Updated: 2026/10/25 16:16:59 by tschwab          ###   ########.ch       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_printf.h"

/**
* @brief
Prints a formatted string to the standard output, like printf.
Resets the write error state, then walks through the format string one step at
a time with printf_loop, until the end of the string or until a write fails.

* @param const char *s
* @param ...
* @return int
*/

int	ft_printf(const char *format, ...)
{
	int		count;
	va_list	args;
	int		result;

	count = 0;
	error_state(-2);
	va_start(args, format);
	while (*format && !error_state(0))
	{
		result = printf_loop(&format, &count, &args);
		if (!result)
			return (va_end(args), -1);
	}
	if (error_state(0))
		return (va_end(args), -1);
	return (va_end(args), count);
}

/**
* @brief
Handles one step of the format string: a plain character or a full %
conversion.
A plain character is printed directly. A % followed by a character is parsed
into a fresh t_flags and sent to printer_manager. A % at the very end of the
string is an invalid format.

* @param const char **s
* @param int *count
* @param va_list *args
* @return int
*/

int	printf_loop(const char **s, int *count, va_list *args)
{
	t_flags	flags;

	if (**s == '%' && *(*s + 1))
	{
		flags = init_flags();
		if (!parser(s, &flags))
			return (0);
		printer_manager(&flags, args, count);
	}
	else
	{
		if (**s == '%')
			return (0);
		*count += print_char(**s);
		(*s)++;
	}
	return (1);
}

/**
* @brief
Remembers whether a write() failed during the current ft_printf call.
The state is kept in a static variable, so it survives between calls without
being a global variable. 

n == -2: reset the state to 0 (no error). write() never returns -2. 
n == -1: a write() failed, set the state to 1. any other
value: only read the state, nothing changes.

* @param int n
* @return int
*/

int	error_state(int n)
{
	static int	error_check;

	if (n == -2)
		error_check = 0;
	else if (n == -1)
		error_check = 1;
	return (error_check);
}
