NAME := task2
CC := gcc

$(NAME): $(NAME).o
	$(CC) $^ -o $@
$(NAME).o: $(NAME).c
	$(CC) -c $<

clean:
	rm -f $(NAME) $(NAME).o
