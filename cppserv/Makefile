.DEFAULT_GOAL = all

NAME=irctest
CXX=c++
CXXFLAGS=-Wall -Wextra -Werror --std=c++98 -MMD -MP
SRCS= \
	 main.cpp \
	 Error.cpp \
	 Parser.cpp \
	 Network.cpp \
	 IRC.cpp \
	 Client.cpp \

OBJS=$(SRCS:.cpp=.o)
DEPS=$(SRCS:.cpp=.d)
-include $(DEPS)

all: $(NAME)

$(NAME): $(OBJS)
	$(CXX) -o $@ $^

%.o: %.cpp
	$(COMPILE.cpp) -c $< -o $@

clean:
	rm -fr $(OBJS) $(DEPS)

fclean: clean
	rm -fr $(NAME)

re: fclean
	make all

.PHONY: all clean fclean re
