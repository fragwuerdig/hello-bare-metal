SRC = paging.c
OBJ = $(SRC:.c=.o)
OUT = mem.o

all: $(OUT)

$(OUT): $(OBJ)
	$(LD) -r -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS64) -c $< -o $@

clean:
	rm -f *.o $(OUT)
