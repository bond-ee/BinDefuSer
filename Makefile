CC=gcc
CFLAGS=
DEPS = crc_calc.h binDfuSer.h
OBJ = crc_calc.o binDfuSer.o

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

binDfuSer: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS)
	
clean:
	rm $(OBJ) binDfuSer
