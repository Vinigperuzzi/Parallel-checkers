CC = gcc
CFLAGS = -Wall
LIBNAME = lib.so
SRC = lib.c
PYCODE = damas.py


$(LIBNAME): $(SRC)
	$(CC) $(CFLAGS) -shared -o $(LIBNAME) $(SRC)
	@echo "Lib compilada com sucesso, você pode rodar o programa com >python damas.py."

clean:
	rm -f $(LIBNAME)
