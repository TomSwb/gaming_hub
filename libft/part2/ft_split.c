/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_split.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tschwab <tschwab@student.42lausanne.ch>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/13 11:40:07 by tschwab           #+#    #+#             */
/*   Updated: 2026/10/15 16:45:42 by tschwab          ###   ########.ch       */
/*                                                                            */
/* ************************************************************************** */

/**
* @brief 
Returns a malloced char ** containing the strings extracted from '*s' 
delimited by 'c'. 

Depends on: 
	ft_calloc();
		external -> malloc();
		
	ft_substr();
		ft_strlen();
		external -> malloc();

Statics:
	ft_word_count();

	ft_extract_word();
		ft_substr();
			ft_strlen();
			external -> malloc();
			
	ft_free_tab();
		external -> free();

* @param char const *s 
* @param char c 
* @return char** 
*/

#include "../libft.h"

static size_t	ft_word_count(char const *s, char c);
static char		*ft_extract_words(char const *s, char c, size_t *ptr);
static void		ft_free_tab(char **tab);

char	**ft_split(char const *s, char c)
{
	char	**result;
	size_t	word_count;
	size_t	i;
	size_t	pos;

	if (!s)
		return (NULL);
	word_count = ft_word_count(s, c);
	result = ft_calloc(word_count + 1, sizeof(char *));
	if (!result)
		return (NULL);
	i = 0;
	pos = 0;
	while (i < word_count)
	{
		result[i] = ft_extract_words(s, c, &pos);
		if (!result[i])
		{
			ft_free_tab(result);
			return (NULL);
		}
		i++;
	}
	return (result);
}

/**
* @brief 
Helper: itering through 's', using 'flag' and detecting 'c', return the counts 
of words inside 's' seperated by 'c'.

* @param s 
* @param c 
* @return size_t 
*/
static size_t	ft_word_count(char const *s, char c)
{
	size_t	flag;
	size_t	word_count;
	size_t	i;

	flag = 1;
	word_count = 0;
	i = 0;
	while (s[i])
	{
		if (s[i] != c && flag == 1)
		{
			flag = 0;
			word_count++;
		}
		else if (s[i] == c && flag == 0)
			flag = 1;
		i++;
	}
	return (word_count);
}

/**
* @brief 
Helper: KEY ALGO: using using pointer algorythmy, stores the pointer position
into the caller 'ptr' variable and returns the extracted word defined by being
delimited by 'c' in 's'.

* @param s 
* @param c 
* @param ptr 
* @return char* 
*/
static char	*ft_extract_words(char const *s, char c, size_t *ptr)
{
	unsigned int	start_pos;
	size_t			len;

	while (s[*ptr] && s[*ptr] == c)
		(*ptr)++;
	start_pos = *ptr;
	while (s[*ptr] && s[*ptr] != c)
		(*ptr)++;
	len = *ptr - start_pos;
	return (ft_substr(s, start_pos, len));
}

/**
* @brief 
Helper: frees all the 'arrays' inside the 'tab' before freeing the 'tab' itself.

* @param tab 
*/
static void	ft_free_tab(char **tab)
{
	size_t	i;

	i = 0;
	while (tab[i] != NULL)
	{
		free(tab[i]);
		i++;
	}
	free(tab);
}
