# Compiler and Linker
CC = gcc
LD = ld
ASM = gcc
CFLAGS32 = -m32 -ffreestanding -nostdlib -O2 -Wall -Wextra
CFLAGS64 = -m64 -ffreestanding -nostdlib -O2 -Wall -Wextra
LDFLAGS = -nostdlib -T linker.ld

# Source Files
#KERNEL_SRC32 = idt.c
KERNEL_SRC64 = gdt64.c kernel.c paging.c idt.c irq.c ringbuffer.c keyboard.c
ASM_SRC = boot.s isr.s

# Object Files
#KERNEL_OBJ32 = $(KERNEL_SRC32:%.c=build/%.o)
KERNEL_OBJ64 = $(KERNEL_SRC64:%.c=build/%.o)
ASM_OBJ = build/boot.o build/isr.o

KERNEL_BIN = kernel.bin

# Default Target
all: iso

# Build rules

build/%.o: %.c
	mkdir -p build
	$(CC) $(CFLAGS64) -c $< -o $@

build/kernel.o: kernel.c
	mkdir -p build
	$(CC) $(CFLAGS64) -c $< -o $@

build/boot.o: boot.S
	mkdir -p build
	$(ASM) -m64 -ffreestanding -c boot.S -o build/boot.o

build/isr.o: isr.S
	mkdir -p build
	$(ASM) -m64 -ffreestanding -c isr.S -o build/isr.o

$(KERNEL_BIN): $(ASM_OBJ) $(KERNEL_OBJ32) $(KERNEL_OBJ64)
	$(LD) $(LDFLAGS) $(ASM_OBJ) $(KERNEL_OBJ32) $(KERNEL_OBJ64) -o build/$(KERNEL_BIN)

# ISO Build
iso: $(KERNEL_BIN)
	mkdir -p iso/boot/grub
	cp build/$(KERNEL_BIN) iso/boot/kernel.bin
	cp grub.cfg iso/boot/grub/grub.cfg
	grub-mkrescue -o mykernel.iso iso

# Clean build artifacts
clean:
	rm -rf build iso mykernel.iso qemu.log boot.s isr.s

# Run in QEMU
run: iso
	qemu-system-x86_64 -cdrom mykernel.iso -no-reboot -no-shutdown -serial mon:stdio -d int,cpu_reset -D qemu.log

