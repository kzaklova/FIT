NAME=bin
OBJFILES=interpret.o precedence.o error_modul.o scanner.o str.o parser.o inst_list.o ial.o main.o

CC=gcc
CFLAGS=-std=c99 -Wall -pedantic  -lm

# univerzalni pravidlo pro generovani vsech objektovych souboru
%.o : %.c
	$(CC) $(CFLAGS) -c $<

# Startovaci pravidlo
all: $(NAME)

# Slinkovani vsech objektovych souboru do jednoho spustitelneho programu .
$(NAME): $(OBJFILES)
	$(CC) $(CFLAGS) $(OBJFILES) -o $@

clean:
	rm $(OBJFILES) $(NAME)

edit:
	gedit main.c precedence.c precedence.h error_modul.h str.h binarytree.c parser.c inst_list.c & 2>/dev/null
