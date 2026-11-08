/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_next_line_utils.c                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tschwab <tschwab@student.42lausanne.ch>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/19 15:56:40 by tschwab           #+#    #+#             */
/*   Updated: 2026/10/15 16:41:52 by tschwab          ###   ########.ch       */
/*                                                                            */
/* ************************************************************************** */

#include "get_next_line.h"

/**
* @brief 
Returns a ssize_t representing the index of 'c' in 's'.
The return value is equal to len of 's' up to 'c'.

* @param char *s 
* @param int c 
* @return ssize_t 
*/
ssize_t	find_c_index(char *s, int c)
{
	size_t	i;

	i = 0;
	if (s == NULL)
		return (0);
	while (s[i])
	{
		if ((unsigned char)s[i] == c)
			return (i);
		i++;
	}
	if (c == '\0')
		return (i);
	return (-1);
}

/**
* @brief 
Returns ssize_t 'reading' indicating eof or not.
Extract the 'buffer' into 'data', ensuring allocation and calling
concatenation function if 'data' isn't empty.

* @param int fd 
* @param char **data 
* @return ssize_t 
*/
ssize_t	extract_buffer(int fd, char **data)
{
	char	*buffer;
	ssize_t	reading;
	ssize_t	total;

	buffer = malloc(PENDING_BUFFER + BUFFER_SIZE + 1);
	if (!buffer || BUFFER_SIZE <= 0)
		return (free(buffer), -1);
	total = 0;
	while (total < PENDING_BUFFER)
	{
		reading = read(fd, buffer + total, BUFFER_SIZE);
		if (reading <= 0)
			break ;
		total += reading;
		buffer[total] = '\0';
		if (find_c_index(buffer + total - reading, '\n') >= 0)
			break ;
	}
	if (reading < 0 || total == 0)
		return (free(buffer), reading);
	if (!cat_buffer_data(buffer, data))
		total = -1;
	return (free(buffer), total);
}

/**
* @brief 
Returns a boolean that confirms the concatenation
of 'buffer' into 'data' if data is not empty
or simply its copy if 'data' is empty.

* @param char *buffer 
* @param char **data 
* @return int 
*/
int	cat_buffer_data(char *buffer, char **data)
{
	char	*temp;
	size_t	i;
	size_t	j;

	temp = malloc(find_c_index(buffer, '\0') + find_c_index(*data, '\0') + 1);
	if (!temp)
		return (0);
	i = 0;
	while (*data && (*data)[i])
	{
		temp[i] = (*data)[i];
		i++;
	}
	j = 0;
	while (buffer[j])
	{
		temp[i + j] = buffer[j];
		j++;
	}
	temp[i + j] = '\0';
	free(*data);
	*data = temp;
	return (1);
}

/**
* @brief 
Returns a success boolean.
Extract the present line in 'data' into the pointed 'line' 
for printing in GNL.

* @param char **line 
* @param char **data 
* @return int 
*/
int	extract_line(char **line, char **data)
{
	ssize_t	len;
	ssize_t	i;

	len = find_c_index(*data, '\n');
	if (len == -1)
		len = find_c_index(*data, '\0');
	else
		len += 1;
	(*line) = malloc(len + 1);
	if (!(*line))
		return (1);
	i = 0;
	while (i < len)
	{
		(*line)[i] = (*data)[i];
		i++;
	}
	(*line)[i] = '\0';
	clean_data(data, len);
	return (0);
}

/**
* @brief 
Frees the **data

* @param char **data 
* @param ssize_t len
* @return void
*/
void	clean_data(char **data, ssize_t len)
{
	ssize_t	i;
	ssize_t	n;

	i = 0;
	n = find_c_index((*data) + len, '\0') + 1;
	while (i < n)
	{
		(*data)[i] = (*data)[len + i];
		i++;
	}
	if ((*data)[0] == '\0')
	{
		free(*data);
		*data = NULL;
	}
}
