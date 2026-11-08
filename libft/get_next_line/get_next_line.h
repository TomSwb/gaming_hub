/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_next_line_bonus.h                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tschwab <tschwab@student.42lausanne.ch>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/19 15:53:07 by tschwab           #+#    #+#             */
/*   Updated: 2026/10/15 16:39:13 by tschwab          ###   ########.ch       */
/*                                                                            */
/* ************************************************************************** */

#ifndef GET_NEXT_LINE_H
# define GET_NEXT_LINE_H

// *** Libraries *** //

// read();
# include <unistd.h>

// malloc();
// free();
# include <stdlib.h>

// size_t
# include <stddef.h>

// *** BUFFER_SIZE handling *** //

# ifndef BUFFER_SIZE
#  define BUFFER_SIZE 1
# endif

# define PENDING_BUFFER 16384L

// *** Struct *** //

typedef struct s_node
{
	int				fd;
	char			*data;
	struct s_node	*next;
}	t_node;

// *** Functions *** //

// get_next_line_bonus.c
char	*get_next_line(int fd);
t_node	*find_node(int fd, t_node **list);
t_node	*create_node(int fd);
void	*free_what_node(int fd, t_node **list);
void	*free_node(t_node *target, t_node **list, int is_head);

// get_next_line_utils_bonus.c
ssize_t	find_c_index(char *s, int c);
ssize_t	extract_buffer(int fd, char **data);
int		cat_buffer_data(char *buffer, char **data);
int		extract_line(char **line, char **data);
void	clean_data(char **data, ssize_t len);

#endif