CC = gcc
CFLAGS = -Wall -Wextra -std=c11

FONTES = main.c Busca.c Gamemod1.c Gamemod2.c GeradorDeMapa.c labirinto.c Animacao.c arte.c Interface.c
OBJETOS = $(FONTES:.c=.o)

ifeq ($(OS),Windows_NT)
    EXEC = labirinto.exe
    RM = del /Q
else
    EXEC = labirinto
    RM = rm -f
endif

all: $(EXEC)

analise: clean
	$(MAKE) CFLAGS="$(CFLAGS) -DANALISE" $(EXEC)

$(EXEC): $(OBJETOS)
	$(CC) $(CFLAGS) -o $(EXEC) $(OBJETOS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

run: $(EXEC)
	./$(EXEC)

clean:
	-$(RM) $(OBJETOS) $(EXEC)

.PHONY: all analise run clean