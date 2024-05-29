.DEFAULT_GOAL=all

DIR_SRCS=srcs/
DIR_INCLUDES=includes/

NAME=ircserv
CXX=c++
CXXFLAGS=-Wall -Wextra -Werror --std=c++98 -Wpedantic 
CPPFLAGS=-MMD -MP -I$(DIR_INCLUDES)

RM=rm -fr

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
	  IRC/ClientEventHandler.cpp \
	  IRC/ServerEventHandler.cpp \
	  IRC/IEventHandler.cpp \
	  IRC/IRC.cpp \
	  Parser/Parser.cpp \
	  Signal/Signal.cpp )

SRCS+=main.cpp
OBJS=$(SRCS:.cpp=.o)
DEPS=$(OBJS:.o=.d)
-include $(DEPS)

ifeq ($(MAKECMDGOALS),debug)
	CPPFLAGS+=-DDEBUG 
	CXXFLAGS+=-g3 -fsanitize=address
endif

%.o: %.cpp
	$(COMPILE.cpp) $(OUTPUT_OPTION) $<

all: $(NAME)

$(NAME): $(OBJS)
	$(LINK.cpp) $(OUTPUT_OPTION) $(OBJS)

debug: all

clean:
	$(RM) $(OBJS) $(DEPS)

fclean: clean
	$(RM) $(NAME)

re: fclean
	$(MAKE) all

.PHONY: all clean fclean re
