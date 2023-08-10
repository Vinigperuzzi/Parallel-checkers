CC = gcc
CFLAGS = -Wall -fPIC -fopenmp -O3
LIBNAME = lib.so
SRC = checkers3.c
PYCODE = damas.py


$(LIBNAME): $(SRC)
	$(CC) $(CFLAGS) -shared -o $(LIBNAME) $(SRC)
	@echo "Lib compilada com sucesso, você pode rodar o programa com >python checkers.py."

clean:
	rm -f $(LIBNAME)
