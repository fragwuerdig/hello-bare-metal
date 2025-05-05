SRC = kthread_t.c scheduler.c
ASM_SRC = switch.S enter.S
OUT = kthread.o
OBJ = $(SRC:.c=.o)
ASM_OBJ = $(ASM_SRC:.S=.o)

all: $(OUT)

$(OUT): $(OBJ) $(ASM_OBJ)
	$(LD) -r -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS64) -c $< -o $@

%.o: %.S
	$(ASM) -m64 -ffreestanding -c $< -o $@

clean:
	rm -f *.o $(OUT)
