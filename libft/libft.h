/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   libft.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tschwab <tschwab@student.42lausanne.ch>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/03 16:41:24 by tschwab           #+#    #+#             */
/*   Updated: 2026/10/25 16:16:19 by tschwab          ###   ########.ch       */
/*                                                                            */
/* ************************************************************************** */

/**
* @brief
Header for complete libft library, defining all required libraries, functions
and a struct.

*/

#ifndef LIBFT_H
# define LIBFT_H

/***** Libraries *****/

// macros
# include <limits.h>
# include <stdint.h>

// read() / write() / close() 
# include <unistd.h> 

// size_t
# include <stddef.h>

// malloc 
# include <stdlib.h>

// gnl & ft_printf
# include "./get_next_line/get_next_line.h"
# include "./ft_printf/ft_printf.h"

/***** Struct *****/

typedef struct s_list
{
	void			*content;
	struct s_list	*next;
}					t_list;

/*****  Functions *****/

/** Part 1 **/

int				ft_isalnum(int c);
int				ft_isalpha(int c);
int				ft_isdigit(int c);
int				ft_isascii(int c);
int				ft_isprint(int c);

int				ft_atoi(char const *nptr);
int				ft_tolower(int c);
int				ft_toupper(int c);

char			*ft_strchr(char const *s, int c);
size_t			ft_strlcat(char *dst, char const *src, size_t size);
size_t			ft_strlcpy(char *dst, char const *src, size_t dsize);
size_t			ft_strlen(char const *s);
int				ft_strncmp(char const *s1, char const *s2, size_t n);
char			*ft_strnstr(char const *big, char const *little, size_t len);
char			*ft_strrchr(char const *s, int c);

void			ft_bzero(void *s, size_t n);
void			*ft_memchr(void const *s, int c, size_t n);
int				ft_memcmp(void const *s1, void const *s2, size_t n);
void			*ft_memcpy(void *dest, void const *src, size_t n);
void			*ft_memmove(void *dest, void const *src, size_t n);
void			*ft_memset(void *s, int c, size_t n);

void			*ft_calloc(size_t n, size_t size);
char			*ft_strdup(char const *s);

/** Part 2 **/

char			*ft_substr(char const *s, unsigned int start, size_t len);
char			*ft_strjoin(char const *s1, char const *s2);
char			*ft_strtrim(char const *s1, char const *set);
char			**ft_split(char const *s, char c);

char			*ft_itoa(int n);

char			*ft_strmapi(char const *s, char (*f)(unsigned int, char));
void			ft_striteri(char *s, void (*f)(unsigned int, char *));

void			ft_putchar_fd(char c, int fd);
void			ft_putstr_fd(char *s, int fd);
void			ft_putendl_fd(char *s, int fd);
void			ft_putnbr_fd(int n, int fd);

/** Part 3 **/

t_list			*ft_lstnew(void *content);
void			ft_lstadd_front(t_list **lst, t_list *node);
void			ft_lstadd_back(t_list **lst, t_list *node);

unsigned int	ft_lstsize(t_list *lst);

t_list			*ft_lstlast(t_list *lst);

void			ft_lstdelone(t_list *lst, void (*del)(void *));
void			ft_lstclear(t_list **lst, void (*del)(void *));

void			ft_lstiter(t_list *lst, void (*f)(void *));
t_list			*ft_lstmap(t_list *lst, void *(*f)(void *),
					void (*del)(void *));

#endif
