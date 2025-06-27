NAME = sockets
CPP = c++
RM = rm -f
FILES = main \
		MySocket \
		ListeningSocket \
		ClientSession \

SRCS = $(addsuffix .cpp, $(FILES))
OBJS = $(addsuffix .o, $(FILES))

all: $(NAME)

%.o: %.cpp network.hpp
	$(CPP) -c $< -o $@

$(NAME): $(OBJS)
	$(CPP) $(OBJS) -o $(NAME)

clean:
	$(RM) $(OBJS)

fclean: clean
	$(RM) $(NAME)

re: fclean all

.PHONY: all clean fcelan re