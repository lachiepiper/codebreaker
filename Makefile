OBJ = crack.o sha256.o generate_guesses.o derivatives.o insert.o
crack: $(OBJ)
	gcc -O3 $(OBJ) -o crack -Wall -lm

all: crack debug

debug: $(OBJ)
	gcc $(OBJ) -o crack_debug -g

dh: dh.o sha256.o
	gcc dh.o sha256.o -o dh -c

crack.o: crack.h

sha256.o: sha256.h

DiffieHellman.o:

generate_guesses.o: crack.h

insert.o: crack.h

derivatives.o: crack.h

dh.o: sha256.h

clean:
	rm *.o
