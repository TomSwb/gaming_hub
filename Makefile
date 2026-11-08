
NAME = gaming_hub

CC = cc
CFLAGS = -Wall -Wextra -Werror

LIBFT_DIR = ./libft
LIBFT = ${LIBFT_DIR}/libft.a

INC_DIR = ./includes
INCLUDES = -I${INC_DIR} -I${LIBFT_DIR}
HEADERS = ${INC_DIR}/gaming_hub.h \
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
