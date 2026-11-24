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

PREFIX ?= ${HOME}/.local

BIN_DIR = ${PREFIX}/bin

DESKTOP = ch.bitminded.GamingHub.desktop

APP_DIR = ${PREFIX}/share/applications

CC = cc
CFLAGS = -Wall -Wextra -Werror

LIBFT_DIR = ./libft
LIBFT = ${LIBFT_DIR}/libft.a

INC_DIR = ./includes
INCLUDES = -I${INC_DIR} -I${LIBFT_DIR}
HEADERS = ${INC_DIR}/gaming_hub.h \
		${INC_DIR}/structs.h ${INC_DIR}/defaults.h \
		${INC_DIR}/computer.h ${INC_DIR}/timer.h \
		${INC_DIR}/game.h ${INC_DIR}/headset.h \
		${INC_DIR}/power.h ${INC_DIR}/library.h \
		${LIBFT_DIR}/libft.h

PKG_CFLAGS := ${shell pkg-config --cflags gtk4 libpulse-mainloop-glib}
PKG_LIBS := ${shell pkg-config --libs gtk4 libpulse-mainloop-glib}

COMPUTER = sources/computer/ram.c \
	sources/computer/cpu.c \
	sources/computer/temp_fan.c \
	sources/computer/gpu.c \
	sources/computer/battery.c \
	sources/computer/sysfs.c \
	sources/computer/stats_build.c

TIMER = sources/timer/settings.c \
	sources/timer/session.c \
	sources/timer/buttons.c \
	sources/timer/show.c \
	sources/timer/alert.c \
	sources/timer/timer_build.c \
	sources/timer/timer_utils.c

GAME = sources/game/game_events.c

HEADSET = sources/headset/pulse.c \
	sources/headset/events.c \
	sources/headset/sink.c \
	sources/headset/source.c \
	sources/headset/default.c \
	sources/headset/volume.c \
	sources/headset/outputs.c \
	sources/headset/outputs_click.c \
	sources/headset/bluez.c \
	sources/headset/bluez_show.c \
	sources/headset/headset_build.c

POWER = sources/power/power.c \
	sources/power/profiles.c \
	sources/power/power_build.c

LIBRARY = sources/library/hub_game.c \
	sources/library/hub_game_props.c \
	sources/library/library_store.c \
	sources/library/library_rows.c \
	sources/library/library_build.c \
	sources/library/keyvalues.c \
	sources/library/keyvalues_2.c \
	sources/library/steam.c \
	sources/library/steam_app.c \
	sources/library/platforms.c \
	sources/library/paths.c \
	sources/library/names.c

SOURCES = sources/main.c \
	sources/utils.c \
	sources/inits.c sources/inits_2.c \
	sources/main_build.c

FILES = ${SOURCES} \
	${COMPUTER} ${TIMER} ${GAME} \
	${HEADSET} ${POWER} ${LIBRARY}

OBJS = ${FILES:.c=.o}

all: libft compile_flags.txt ${NAME}

libft:
	@${MAKE} -C ${LIBFT_DIR} --no-print-directory

${NAME}: ${LIBFT} ${OBJS}
	@echo "LD ${NAME}"
	@${CC} ${CFLAGS} ${OBJS} ${PKG_LIBS} ${LIBFT} -o ${NAME}

compile_flags.txt: Makefile
	@printf '%s\n' ${CFLAGS} ${PKG_CFLAGS} ${INCLUDES} > $@

%.o: %.c ${HEADERS}
	@echo "CC $<"
	@${CC} ${CFLAGS} ${PKG_CFLAGS} ${INCLUDES} -c $< -o $@

clean:
	@rm -f ${OBJS}
	@${MAKE} -C ${LIBFT_DIR} clean --no-print-directory

fclean: clean
	@echo "F-Cleaned"
	@rm -f ${NAME} compile_flags.txt
	@${MAKE} -C ${LIBFT_DIR} fclean --no-print-directory

re: fclean all

install: all
	install -D -m 755 ${NAME} ${BIN_DIR}/${NAME}
	mkdir -p ${APP_DIR}
	printf '%s\n' '[Desktop Entry]' 'Type=Application' 'Name=Gaming Hub' \
		'Comment=Gaming session companion' \
		'Exec=${BIN_DIR}/${NAME}' \
		'Icon=input-gaming' 'Categories=Game;' > ${APP_DIR}/${DESKTOP}
	-update-desktop-database ${APP_DIR}

uninstall:
	rm -f ${APP_DIR}/${DESKTOP}
	rm -f ${BIN_DIR}/${NAME}
	-update-desktop-database ${APP_DIR}

.PHONY: all clean fclean re libft install uninstall
