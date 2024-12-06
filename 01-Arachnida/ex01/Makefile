NAME = spider
SRC = $(wildcard *.c)
OBJ_DIR = obj
OBJ = $(patsubst %.c, $(OBJ_DIR)/%.o, $(SRC))
CC = gcc
CFLAGS = -g -I /usr/local/include 


all: $(NAME)

$(OBJ_DIR):
	@mkdir -p $(OBJ_DIR)

$(NAME): $(OBJ)
	$(CC) $(CFLAGS) $(OBJ) -o $(NAME) -lssl -lcrypto
	@echo "Spider compiled successfully ✅"

$(OBJ_DIR)/%.o: %.c | $(OBJ_DIR) 
	$(CC) $(CFLAGS) -c $< -o $@
	
clean:
		@rm -rf $(OBJ_DIR)
		@echo "Spider object files cleaned successfully 🗑"
fclean:
		@rm -rf $(OBJ_DIR)
		@rm -rf $(NAME)
		@echo "Spider object files and Spider cleaned successfully 🗑"

re: fclean all