NAME = webserv

SRCS =	main.cpp gnl/get_next_line.cpp gnl/get_next_line_utils.cpp

HDRS = gnl/get_next_line.hpp

OBJS = $(SRCS:.cpp=.o)

FLAGS = -O3 -std=c++98 -Wno-c++11-compat-deprecated-writable-strings

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
