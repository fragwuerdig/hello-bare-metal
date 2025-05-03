SRC = terminal.c print.c format.c
OBJ = $(SRC:.c=.o)
OUT = display.o

all: $(OUT)

$(OUT): $(OBJ)
	$(LD) -r -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS64) -c $< -o $@

clean:
	rm -f *.o $(OUT)
