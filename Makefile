NAME = webserv


SRCS =	src/main.cpp \
 		src/cgi/CGI.cpp \
 		src/other/utils.cpp \
 		src/parse/formatConfigFile.cpp \
	    src/parse/ListenConfig.cpp \
	    src/parse/LocationConfig.cpp \
	    src/parse/serverConfig.cpp \
	    src/parse/print_util.cpp \
	    src/http/ClientRequestParse.cpp \
	    src/http/ClientRequestAnalyse.cpp \
	    src/http/ClientResponse.cpp \
	    src/http/ClientRequestRead.cpp \

HDRS =  src/server/Webserv.hpp\
 		src/main.hpp \
 		src/server/PortServer.hpp \
 		src/server/VirtualServerConfig.hpp \
 		src/http/Request.hpp \
 		src/http/Response.hpp \
 		src/http/Client.hpp \
 		src/server/VirtualServerConfigDirectory.hpp \
 		src/http/AutoIndex.hpp \
 		src/cgi/CGI.hpp \
 		src/parse/errorCodes.hpp \
        src/parse/formatConfigFile.hpp \
        src/parse/IParse.hpp \
        src/parse/LocationConfig.hpp \
        src/parse/serverConfig.hpp


OBJS = $(SRCS:.cpp=.o)

FLAGS = -O3 -g -Wall -Wextra -Werror -std=c++98

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
