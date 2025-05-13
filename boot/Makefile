SRC = multiboot2.c
ASM_SRC = boot_asm.S
OUT = boot.o
OBJ = $(SRC:.c=.o)
ASM_OBJ = $(ASM_SRC:.S=.o)

all: $(OUT)

$(OUT): $(OBJ) $(ASM_OBJ)
	$(LD) -r -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS64) -c $< -o $@

%.o: %.S
	$(ASM) $(CFLAGS64) -c $< -o $@

clean:
	rm -f *.o $(OUT)
