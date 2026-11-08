/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_lstmap.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tschwab <tschwab@student.42lausanne.ch>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/18 18:09:06 by tschwab           #+#    #+#             */
/*   Updated: 2026/10/15 16:46:45 by tschwab          ###   ########.ch       */
/*                                                                            */
/* ************************************************************************** */

/**
* @brief 
Returns a new list created by applying the given function f to the content of 
each node of the original list. If an allocation fails, clears the whole new 
list using the given del function and returns NULL.

Depends on:
	ft_lstnew();
		malloc();
	ft_lstclear();
		free();
	ft_lstadd_back();

* @param t_list *lst 
* @param void *(*f)(void *) 
* @param void (*del)(void *) 
* @return t_list* 
*/

#include "../libft.h"

t_list	*ft_lstmap(t_list *lst, void *(*f)(void *), void (*del)(void *))
{
	t_list	*result;
	t_list	*node;
	void	*content;

	if (!lst || !f)
		return (NULL);
	result = NULL;
	while (lst)
	{
		content = f(lst->content);
		node = ft_lstnew(content);
		if (!node)
		{
			del(content);
			ft_lstclear(&result, del);
			return (NULL);
		}
		ft_lstadd_back(&result, node);
		lst = lst->next;
	}
	return (result);
}
