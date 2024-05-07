#-------------------------------------------

BLACK       =   "\033[0;30m"
GRAY        =   "\033[1;30m"
RED         =   "\033[1;31m"
GREEN       =   "\033[0;32m"
YELLOW      =   "\033[1;33m"
PURPLE      =   "\033[0;35m"
CYAN        =   "\033[1;36m"
WHITE       =   "\033[1;37m"
EOC         =   "\033[0;0m"
LINE_CLEAR  =   "\x1b[1A\x1b[M"

GREEN_BACK	=	"\033[102m"

#-------------------------------------------

CXX = c++

CXXFLAGS = -Wall -Wextra -Werror -std=c++98

RM = rm -rf

NAME = ircserv

#-------------------------------------------

SRCS = main.cpp Server.cpp Channel.cpp Client.cpp Parser.cpp

OBJS = $(SRCS:.cpp=.o)

#-------------------------------------------

all: $(NAME)

%.o: %.cpp
	@$(CXX) $(CXXFLAGS) -O3 -c $< -o $@

$(NAME): $(OBJS)
	@$(CXX) $(CXXFLAGS) $^ -o $@
	@echo $(GREEN)"\n==========================================================\n"$(EOC)
	@echo $(YELLOW)"                        ft_irc DONE"$(EOC)
	@echo $(GREEN)"\n==========================================================\n"$(EOC)

clean:
	@$(RM) $(OBJS)

fclean:
	@make clean
	@$(RM) $(NAME)

re:
	@make fclean
	@make all