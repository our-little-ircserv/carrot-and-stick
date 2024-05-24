.DEFAULT_GOAL=all

NAME=irctest
CXX=c++
CXXFLAGS=-DDEBUG#-Wall -Wextra -Werror
CPPFLAGS=-MMD -MP -Ilibircserv/includes

RM=rm -fr

SRCS= \
	  main.cpp \

OBJS=$(SRCS:.cpp=.o)
DEPS=$(OBJS:.o=.d)

-include $(DEPS)

all: $(NAME)

%.o: %.cpp
	make -C libircserv
	$(COMPILE.cpp) $(OUTPUT_OPTION) $<

$(NAME): $(OBJS) libircserv/
	$(CXX) -o $(NAME) $(OBJS) -lircserv -Llibircserv

clean:
	make clean -C libircserv
	$(RM) $(OBJS) $(DEPS)

fclean: clean
	make fclean -C libircserv
	$(RM) $(NAME)

re: fclean all

.PHONY: all clean fclean re
