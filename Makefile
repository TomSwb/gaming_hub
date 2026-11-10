# **************************************************************************** #
#                                                                              #
#                                              :::::::    :::     :::  :::::   #
#   Makefile                                   :+:   :+:  :+:+: :+:+:   :+:    #
#                                              +#++:+#+   +#+ +:+ +#+   +#+    #
#   By: TomSwb <github.com/TomSwb>             +#+   +#+  +#+     +#+   +#+    #
#                                              ########   ###     ###  #####   #
#   Created: 2026/11/10 23:08:32 by TomSwb                                     #
#   Updated: 2026/11/10 23:08:32 by TomSwb                     bitminded.ch    #
#                                                                              #
# **************************************************************************** #

NAME = gaming_hub

CC = cc
CFLAGS = -Wall -Wextra -Werror

LIBFT_DIR = ./libft
LIBFT = ${LIBFT_DIR}/libft.a

INC_DIR = ./includes
INCLUDES = -I${INC_DIR} -I${LIBFT_DIR}
HEADERS = ${INC_DIR}/gaming_hub.h \
		${INC_DIR}/structs.h \
		${INC_DIR}/computer.h \
		${LIBFT_DIR}/libft.h

GTK_CFLAGS := ${shell pkg-config --cflags gtk4}
GTK_LIBS := ${shell pkg-config --libs gtk4}

COMPUTER = sources/computer/ram.c \
	sources/computer/cpu.c \
	sources/computer/temp_fan.c \
	sources/computer/gpu.c \
	sources/computer/battery.c \
	sources/computer/sysfs.c

SRCS = sources/main.c \
	sources/utils.c \
	${COMPUTER}

OBJS = ${SRCS:.c=.o}

all: libft compile_flags.txt ${NAME}

libft:
	${MAKE} -C ${LIBFT_DIR}

${NAME}: ${LIBFT} ${OBJS}
	${CC} ${CFLAGS} ${OBJS} ${GTK_LIBS} ${LIBFT} -o ${NAME}

compile_flags.txt: Makefile
	printf '%s\n' ${CFLAGS} ${GTK_CFLAGS} ${INCLUDES} > $@

%.o: %.c ${HEADERS}
	${CC} ${CFLAGS} ${GTK_CFLAGS} ${INCLUDES} -c $< -o $@

clean:
	rm -f ${OBJS}
	${MAKE} -C ${LIBFT_DIR} clean

fclean: clean
	rm -f ${NAME} compile_flags.txt
	${MAKE} -C ${LIBFT_DIR} fclean

re: fclean all

.PHONY: all clean fclean re libft
