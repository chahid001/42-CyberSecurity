NAME = spider
SRC = $(wildcard *.c)
OBJ = $(SRC:.c=.o)
CC = gcc
CFLAGS = -I /usr/local/include -L/usr/local/lib -lssl -lcrypto


all: $(NAME)

$(NAME): $(OBJ)
	$(CC) $(CFLAGS) $(OBJ) -o $(NAME)
	@echo "Spider compiled successfully ✅"

%.o: %.c 
	$(CC) $(CFLAGS) -c $< 
	
clean:
		@rm -rf $(OBJ)
		@echo "Spider object files cleaned successfully 🗑"
fclean:
		@rm -rf $(OBJ)
		@rm -rf $(NAME)
		@echo "Spider object files and Spider cleaned successfully 🗑"

re: fclean all