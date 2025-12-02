export PATH := $(HOME)/opt/cross/bin:/usr/bin:/bin:$(PATH)

.PHONY: all clean fclean re

SRC_DIR := src
OBJ_DIR := obj
SRCS := \
	kernel/kernel.c	\
	arch/x86/asm/boot/boot.s \
	arch/x86/asm/gdt/gdtReload.s \
	arch/x86/asm/boot/multiboot2_header.s \
	arch/x86/boot/multiboot2.c \
	arch/x86/boot/initHighHalf.c \
	arch/x86/gdt/gdtInit.c \
	arch/x86/graphics/framebuffer.c \
	arch/x86/hwio/ports.c \
	arch/x86/memory/pmm/pmm.c \
	arch/x86/memory/map/memoryMap.c \
	drivers/x86/serial/serial.c \
	lib/string.c
SRCS := $(SRCS:%=$(SRC_DIR)/%)
OBJS := $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(filter %.c, $(SRCS))) \
        $(patsubst $(SRC_DIR)/%.s, $(OBJ_DIR)/%.o, $(filter %.s, $(SRCS)))

AS := i686-elf-as
CC := i686-elf-gcc
CFLAGS := -std=gnu99 -ffreestanding -O2 -Wall -Wextra -Isrc/include
LINKER := i686-elf-gcc
LDFLAGS := -ffreestanding -T linker.ld -O2 -nostdlib -lgcc -Isrc/include

RM := rm -f
DIR_DUP = mkdir -p $(@D)

all: build/SnakeOS.iso

build/SnakeOS.iso: SnakeOS.elf
	$(DIR_DUP)
	mv SnakeOS.elf build/iso/boot
	grub-mkrescue -o build/SnakeOS.iso build/iso

SnakeOS.elf: $(OBJS)
	$(LINKER) $(LDFLAGS) -o $@ $^

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	$(DIR_DUP)
	$(CC) $(CFLAGS) -c -o $@ $<

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.s
	$(DIR_DUP)
	$(AS) -o $@ $<

clean: 
	$(RM) -r obj

fclean:
	$(MAKE) clean
	$(RM) build/SnakeOS.iso

re:
	$(MAKE) fclean
	$(MAKE) all
