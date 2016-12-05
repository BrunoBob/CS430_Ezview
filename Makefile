COMPIL = gcc
FLAG = -Wall -lGLESv2 -lglfw -lm
NAME = ezview

all: $(NAME)

$(NAME): $(NAME).c
	$(COMPIL) -o $(NAME) $(NAME).c $(FLAG)

clean:
	rm $(NAME)
