NAME = webserv


SRCS =	main.cpp CGI.cpp utils.cpp parse/formatConfigFile.cpp \
                           parse/ListenConfig.cpp \
                           parse/LocationConfig.cpp \
                           parse/serverConfig.cpp \
                           parse/tools.cpp

#HDRS = Webserv.hpp main.hpp ListenSocketConfigDirectory.hpp ListenSocketConfig.hpp Request.hpp Response.hpp Client.hpp ListenSocket.hpp AutoIndex.hpp

HDRS = Webserv.hpp\
 		main.hpp \
 		PortServer.hpp \
 		VirtualServerConfig.hpp \
 		Request.hpp \
 		Response.hpp \
 		Client.hpp \
 		VirtualServerConfigDirectory.hpp \
 		AutoIndex.hpp \
 		CGI.hpp \
 		parse/errorCodes.hpp \
        parse/formatConfigFile.hpp \
        parse/IParse.hpp \
        parse/LocationConfig.hpp \
        parse/serverConfig.hpp


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
