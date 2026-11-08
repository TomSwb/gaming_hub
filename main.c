/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tschwab <tschwab@student.42lausanne.ch>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/01 23:17:55 by tschwab           #+#    #+#             */
/*   Updated: 2026/10/25 16:16:25 by tschwab          ###   ########.ch       */
/*                                                                            */
/* ************************************************************************** */

#include "template.h"
#include <fcntl.h>

int	main(int ac, char **av)
{
	int		fd;
	char	*s;

	(void) ac;
	fd = open(av[1], O_RDONLY);
	s = get_next_line(fd);
	ft_printf("Test this: %s = %d len\n", s, ft_strlen(s));
	close(fd);
}
