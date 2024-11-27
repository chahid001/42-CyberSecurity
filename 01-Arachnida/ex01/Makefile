NAME = spider
SRC = $(wildcard *.c)
OBJ = $(SRC:.c=.o)
CC = gcc


all: $(NAME)

$(NAME): $(OBJ)
	$(CC) $(OBJ) -o $(NAME)
	@echo "Spider compiled successfully ✅"

%.o: %.c 
	$(CC) -c $< 
	
clean:
		@rm -rf $(OBJ)
		@echo "Spider object files cleaned successfully 🗑"
fclean:
		@rm -rf $(OBJ)
		@rm -rf $(NAME)
		@echo "Spider object files and Spider cleaned successfully 🗑"

re: fclean all