/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_printf.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tschwab <tschwab@student.42lausanne.ch>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/29 17:53:57 by tschwab           #+#    #+#             */
/*   Updated: 2026/10/25 16:17:03 by tschwab          ###   ########.ch       */
/*                                                                            */
/* ************************************************************************** */

#ifndef FT_PRINTF_H
# define FT_PRINTF_H

// *** Libraries *** //

// va_arg functions & type
# include <stdarg.h>

// write();
# include <unistd.h>

// uintptr_t type
# include <stdint.h>

// *** Struct *** //

typedef struct s_flags
{
	int		left_align;
	int		zero_padding;
	int		positive_sign;
	int		alt_hexa;
	int		space;
	int		neg_sign;

	int		min_width;
	int		precision;
	int		precision_len;

	char	converter;
}	t_flags;

// *** Functions *** //

// ft_printf.c
int			ft_printf(const char *format, ...);
int			printf_loop(const char **s, int *count, va_list *args);
int			error_state(int n);

//ft_printf_managers.c
void		printer_manager(t_flags *flags, va_list *args, int *count);
void		printer_manager_char(t_flags *flags, va_list *args, int *count);
void		printer_manager_decimal(t_flags *flags, va_list *args, int *count);
void		printer_manager_hexa(t_flags *flags, va_list *args, int *count);
void		printer_manager_address(t_flags *flags, va_list *args, int *count);

// ft_printf_parsing.c
t_flags		init_flags(void);
int			parser(const char **s, t_flags *flags);
void		parse_min_width_precison(t_flags *flags, const char **s);
void		parsing_check(t_flags *flags);

// ft_printf_parsing_utils.c
int			is_converter(char c);
int			is_num(char c);
int			is_flag(char c);

// ft_printf_printing.c
int			print_char(char value);
int			print_s(const char *value, t_flags *flags);
int			print_deci(long long value, int len, t_flags *flags);
int			print_hexa(long long value, char *base, t_flags *flags);
int			print_address(uintptr_t value);

// ft_printf_printing_utils.c
int			print_padding(t_flags *flags, int len_value);
int			print_prefix(t_flags *flags);
int			print_precision(int len);
int			print_nothing(int len, t_flags *flags);
long long	def_div(long long value, t_flags *flags);

// ft_printf_len_utils.c
int			s_len(char *s, t_flags *flags);
int			deci_len(long long value, t_flags *flags);
int			pr_len(int pr_len, t_flags *flags);
int			hexa_len(long long value, t_flags *flags);
int			address_len(uintptr_t value);

#endif
