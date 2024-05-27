.DEFAULT_GOAL=all

DIR_SRCS=srcs/
DIR_INCLUDES=includes/

NAME=ircserv
CXX=c++
CXXFLAGS=-Wall -Wextra -Werror --std=c++98
CPPFLAGS=-MMD -MP -I$(DIR_INCLUDES)

RM=rm -fr
-include $(DEPS)

SRCS=$(addprefix $(DIR_SRCS), \
	  Channel/Channel.cpp \
	  Client/Client.cpp \
	  Command/Command.cpp \
	  Command/Invite.cpp \
	  Command/Join.cpp \
	  Command/Kick.cpp \
	  Command/Mode.cpp \
	  Command/Nick.cpp \
	  Command/Pass.cpp \
	  Command/Privmsg.cpp \
	  Command/Topic.cpp \
	  Command/User.cpp \
	  Command/Part.cpp \
	  Command/Quit.cpp \
	  Command/Names.cpp \
	  ErrorReply/FatalError.cpp \
	  ErrorReply/Reply.cpp \
	  IRC/IRC.cpp \
	  Parser/Parser.cpp \
	  Signal/Signal.cpp )

SRCS+=main.cpp
OBJS=$(SRCS:.cpp=.o)
DEPS=$(OBJS:.o=.d)

ifeq ($(MAKECMDGOALS),debug)
	CXXFLAGS+=-DDEBUG -g3 -fsanitize=address
endif

%.o: %.cpp
	$(COMPILE.cpp) $(OUTPUT_OPTION) $<

all: $(NAME)

$(NAME): $(OBJS)
	$(LINK.cpp) -o $(NAME) $(OBJS)

debug: all

clean:
	$(RM) $(OBJS) $(DEPS)

fclean: clean
	$(RM) $(NAME)

re: fclean all

.PHONY: all clean fclean re
