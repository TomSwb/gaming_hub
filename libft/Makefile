# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: tschwab <tschwab@student.42lausanne.ch>    +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2026/10/01 23:23:16 by tomswb            #+#    #+#              #
#    Updated: 2026/10/25 16:04:54 by tschwab          ###   ########.ch        #
#                                                                              #
# **************************************************************************** #

NAME = libft.a

PART1 = ./part1/ft_atoi.c \
		./part1/ft_bzero.c \
		./part1/ft_calloc.c \
		./part1/ft_isalnum.c \
		./part1/ft_isalpha.c \
		./part1/ft_isascii.c \
		./part1/ft_isdigit.c \
		./part1/ft_isprint.c \
		./part1/ft_memchr.c \
		./part1/ft_memcmp.c \
		./part1/ft_memcpy.c \
		./part1/ft_memmove.c \
		./part1/ft_memset.c \
		./part1/ft_strchr.c \
		./part1/ft_strdup.c \
		./part1/ft_strlcat.c \
		./part1/ft_strlcpy.c \
		./part1/ft_strlen.c \
		./part1/ft_strncmp.c \
		./part1/ft_strnstr.c \
		./part1/ft_strrchr.c \
		./part1/ft_tolower.c \
		./part1/ft_toupper.c

PART2 = ./part2/ft_itoa.c \
		./part2/ft_putchar_fd.c \
		./part2/ft_putendl_fd.c \
		./part2/ft_putnbr_fd.c \
		./part2/ft_putstr_fd.c \
		./part2/ft_split.c \
		./part2/ft_striteri.c \
		./part2/ft_strjoin.c \
		./part2/ft_strmapi.c \
		./part2/ft_strtrim.c \
		./part2/ft_substr.c

PART3 = ./part3/ft_lstadd_back.c \
		./part3/ft_lstadd_front.c \
		./part3/ft_lstclear.c \
		./part3/ft_lstdelone.c \
		./part3/ft_lstiter.c \
		./part3/ft_lstlast.c \
		./part3/ft_lstmap.c \
		./part3/ft_lstnew.c \
		./part3/ft_lstsize.c

GNL = ./get_next_line/get_next_line.c \
		./get_next_line/get_next_line_utils.c
		
FT_PRINTF = ./ft_printf/ft_printf.c \
		./ft_printf/ft_printf_managers.c \
		./ft_printf/ft_printf_parsing.c \
		./ft_printf/ft_printf_parsing_utils.c \
		./ft_printf/ft_printf_printing.c \
		./ft_printf/ft_printf_printing_utils.c \
		./ft_printf/ft_printf_len_utils.c

SOURCES = ${PART1} ${PART2} ${PART3} ${GNL} ${FT_PRINTF}
		
CC = cc

CFLAGS = -Wall -Werror -Wextra

OBJECTS = ${SOURCES:.c=.o}

AR = ar -rcs

RM = rm -f

%.o: %.c libft.h get_next_line/get_next_line.h ft_printf/ft_printf.h
	${CC} ${CFLAGS} -c $< -o $@

all: ${NAME}

${NAME}: ${OBJECTS}
		${AR} ${NAME} ${OBJECTS}

clean: 
		${RM} ${OBJECTS}

fclean: clean
		${RM} ${NAME}

re: fclean all

.PHONY: all clean fclean re
