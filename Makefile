COMPIL = gcc
FLAG = -Wall -lGLESv2 -lglfw -lm
NAME = ezview

all: $(NAME)

ppmread.o: ppmread.c
	$(COMPIL) -c ppmread.c $(FLAG)

$(NAME).o: $(NAME).c
	$(COMPIL) -c $(NAME).c $(FLAG)

$(NAME): $(NAME).o ppmread.o
	$(COMPIL) -o $(NAME) $(NAME).o ppmread.o $(FLAG)

clean:
	rm $(NAME) *.o
