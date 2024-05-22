.DEFAULT_GOAL=all

DIR_SRCS=srcs/
DIR_INCLUDES=includes/
DIR_OBJS=objs/

NAME=libircserv.a
CXX=c++
CXXFLAGS=#-Wall -Wextra -Werror
CPPFLAGS=-MMD -MP -I$(DIR_INCLUDES)

RM=rm -fr
AR=ar
ARFLAGS=rc
-include $(DEPS)

SRCS= $(addprefix $(DIR_SRCS), \
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
	  ErrorReply/FatalError.cpp \
	  ErrorReply/Reply.cpp \
	  IRC/IRC.cpp \
	  Parser/Parser.cpp \
	  Signal/Signal.cpp \
	  Client/Client.cpp )

OBJS=$(SRCS:.cpp=.o)
DEPS=$(OBJS:.o=.d)

#OBJS=$(addprefix .objs/, $(notdir $(SRCS:.cpp=.o)))


#.objs:
#	mkdir -p $@

%.o: %.cpp
	$(COMPILE.cpp) $(OUTPUT_OPTION) $<

all: $(NAME)

$(NAME): $(OBJS)
	$(AR) $(ARFLAGS) $(NAME) $(OBJS)

clean:
	$(RM) $(OBJS) $(DEPS)

fclean: clean
	$(RM) $(NAME)

re: fclean all

.PHONY: all clean fclean re
