NAME = webserv

SRCS =	main.cpp Webserv.cpp utils.cpp

HDRS = Webserv.hpp main.hpp ListenSocketConfigDirectory.hpp ListenSocketConfig.hpp Request.hpp Response.hpp Client.hpp ListenSocket.hpp AutoIndex.hpp

OBJS = $(SRCS:.cpp=.o)

FLAGS = -O3 -Wno-c++11-compat-deprecated-writable-strings

COMP = g++

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
