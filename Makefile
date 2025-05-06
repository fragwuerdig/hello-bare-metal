
CURDIR := $(shell pwd)

# Compiler and Linker
CC = gcc
LD = ld
ASM = gcc
export CC LD ASM

# Compiler Flags
CFLAGS32 = -m32 -ffreestanding -nostdlib -O2 -Wall -Wextra -I$(CURDIR)/include
CFLAGS64 = -m64 -ffreestanding -nostdlib -O2 -Wall -Wextra -I$(CURDIR)/include
LDFLAGS = -nostdlib -T linker.ld
export CFLAGS32 CFLAGS64 LDFLAGS

# Subdirectories
SUBDIRS = display klib kthread mem
OBJSUFFIX = $(patsubst %, /%.o, $(SUBDIRS))
OBJSUBDIRS = $(join $(SUBDIRS), $(OBJSUFFIX))

# Source Files
#KERNEL_SRC32 = idt.c
KERNEL_SRC64 = gdt64.c kernel.c idt.c irq.c ringbuffer.c keyboard.c
ASM_SRC = boot.s isr.s

# Object Files
KERNEL_OBJ64 = $(KERNEL_SRC64:%.c=build/%.o)
ASM_OBJ = build/boot.o build/isr.o

KERNEL_BIN = kernel.bin

print-%:
	@echo '$* = $($*)'

# Default Target
all: iso

# Build rules

$(OBJSUBDIRS):
	$(MAKE) -C $(notdir $(basename $@))

build/%.o: %.c
	mkdir -p build
	$(CC) $(CFLAGS64) -c $< -o $@

build/kernel.o: kernel.c
	mkdir -p build
	$(CC) $(CFLAGS64) -c $< -o $@

build/boot.o: boot.S
	mkdir -p build
	$(ASM) -m64 -ffreestanding -Iinclude -c boot.S -o build/boot.o

build/isr.o: isr.S
	mkdir -p build
	$(ASM) -m64 -ffreestanding -c isr.S -o build/isr.o

$(KERNEL_BIN): $(ASM_OBJ) $(KERNEL_OBJ64) $(OBJSUBDIRS)
	$(LD) $(LDFLAGS) $(ASM_OBJ) $(KERNEL_OBJ64) $(OBJSUBDIRS) -o build/$(KERNEL_BIN)

# ISO Build
iso: $(KERNEL_BIN)
	mkdir -p iso/boot/grub
	cp build/$(KERNEL_BIN) iso/boot/kernel.bin
	cp grub.cfg iso/boot/grub/grub.cfg
	grub-mkrescue -o mykernel.iso iso

# Clean build artifacts
clean:
	rm -rf build iso mykernel.iso qemu.log boot.s isr.s
	for dir in $(SUBDIRS); do \
		$(MAKE) -C $$dir clean; \
	done

# Run in QEMU
run: iso
	qemu-system-x86_64 -cdrom mykernel.iso -no-reboot -no-shutdown -serial mon:stdio -d int,cpu_reset -D qemu.log

