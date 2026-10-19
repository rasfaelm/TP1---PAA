
CC = gcc
CFLAGS = -Wall -Wextra -std=c11
EXEC = labirinto.exe

FONTES = teste.c Gamemod1.c Gamemod2.c GeradorDeMapa.c
OBJETOS = teste.o Gamemod1.o Gamemod2.o GeradorDeMapa.o

# Detecta o sistema operacional
ifeq ($(OS),Windows_NT)
    RM = del /Q
    EXEC = labirinto.exe
else
    RM = rm -f
    EXEC = labirinto
endif

all: $(EXEC)

$(EXEC): $(OBJETOS)
	$(CC) $(CFLAGS) -o $(EXEC) $(OBJETOS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

run: $(EXEC)
	./$(EXEC)

clean:
ifeq ($(OS),Windows_NT)
	-$(RM) $(OBJETOS) $(EXEC)
else
	$(RM) $(OBJETOS) $(EXEC)
endif

.PHONY: all run clean
