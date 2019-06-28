COMP=gcc
CFLAGS=-O3 -Wa,--execstack -Wall -Wextra
OUT=prog
SRC=gera.c main.c

normal:
	echo "Compilando..."
	$(COMP) $(CFLAGS) -o $(OUT) $(SRC)
	echo "Compilação terminada. Cheque se houve erros."
