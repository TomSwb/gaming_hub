# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: tschwab <tschwab@student.42lausanne.ch>    +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2026/10/01 23:17:43 by tomswb            #+#    #+#              #
#    Updated: 2026/10/25 16:13:35 by tschwab          ###   ########.ch        #
#                                                                              #
# **************************************************************************** #

NAME = template

CC = cc
CFLAGS = -Wall -Wextra -Werror

LIBFT_DIR = ./libft
LIBFT = ${LIBFT_DIR}/libft.a

INC_DIR = ./includes
INCLUDES = -I${INC_DIR} -I${LIBFT_DIR}
HEADERS = ${INC_DIR}/template.h \
		${LIBFT_DIR}/libft.h

SRCS = main.c

OBJS = ${SRCS:.c=.o}

all: libft ${NAME}

libft:
	${MAKE} -C ${LIBFT_DIR}

${NAME}: ${LIBFT} ${OBJS}
	${CC} ${CFLAGS} ${OBJS} ${LIBFT} -o ${NAME}

%.o: %.c ${HEADERS}
	${CC} ${CFLAGS} ${INCLUDES} -c $< -o $@

clean:
	rm -f ${OBJS}
	${MAKE} -C ${LIBFT_DIR} clean

fclean: clean
	rm -f ${NAME}
	${MAKE} -C ${LIBFT_DIR} fclean

re: fclean all

.PHONY: all clean fclean re libft