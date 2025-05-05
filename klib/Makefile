SRC = string.c
OBJ = $(SRC:.c=.o)
OUT = klib.o

all: $(OUT)

$(OUT): $(OBJ)
	$(LD) -r -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS64) -c $< -o $@

clean:
	rm -f *.o $(OUT)
