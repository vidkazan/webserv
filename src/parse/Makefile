NAME	= program

CC		= g++

CFLAGS	= -Wall -Wextra -Werror

HEADERS := $(wildcard *.hpp)

SRCS := $(wildcard *.cpp)

OBJS := $(patsubst %.cpp,%.o,$(SRCS))

%.o: %.cpp
	$(CC) $(FAGS) -c $< -o $@

all:	${NAME}

${NAME}:	${OBJS} ${HEADERS}
			${CC} ${CFLAGS} -o ${NAME} ${OBJS}

clean:
			@echo "Cleaning: ${OBJS}"
			@rm -rf ${OBJS}

fclean:		clean
			@rm -rf ${NAME}
			@echo "\033[0mdelete "${NAME}"... \033[32mok\033[0m"

re:			fclean all

.phony: all clean fclean re