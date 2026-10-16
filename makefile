
CC = gcc
CFLAGS = -Wall -Wextra -std=c11
EXEC = labirinto.exe

FONTES = teste.c Gamemod1.c Gamemod2.c
OBJETOS = teste.o Gamemod1.o Gamemod2.o

all: $(EXEC)

$(EXEC): $(OBJETOS)
	$(CC) $(CFLAGS) -o $(EXEC) $(OBJETOS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

run: $(EXEC)
	./$(EXEC)

clean:
	rm -f $(OBJETOS) $(EXEC)

.PHONY: all run clean
