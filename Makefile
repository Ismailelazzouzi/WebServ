NAME = webserv
CPP = c++
RM = rm -f
FILES = MySocket \
		ListeningSocket \
		ConfigParser \
		ClientSession \
		RequestParser \
		ResponseBuilder \
		main \

SRCS = $(addsuffix .cpp, $(FILES))
OBJS = $(addsuffix .o, $(FILES))

all: $(NAME)

%.o: %.cpp
	$(CPP) -c $< -o $@

$(NAME): $(OBJS)
	$(CPP) $(OBJS) -o $(NAME)

clean:
	$(RM) $(OBJS)

fclean: clean
	$(RM) $(NAME)

re: fclean all

.PHONY: all clean fcelan re