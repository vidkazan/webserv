NAME = webserv

SRCS =	main.cpp

HDRS = Webserv.hpp main.hpp

OBJS = $(SRCS:.cpp=.o)

FLAGS = -O3 -std=c++98 -Wno-c++11-compat-deprecated-writable-strings -Wall -Wextra -Werror

COMP = c++

%.o: %.cpp		$(HDRS)
				@$(COMP) $(FLAGS) -c $< -o $@

all:			$(NAME)

$(NAME):		$(OBJS)
				@$(COMP) $(FLAGS) $(OBJS) -o $@

clean:
				@rm -f $(OBJS)

fclean:			clean
				@rm -f $(NAME)

re:				fclean all


.PHONY:			all clean fclean re
