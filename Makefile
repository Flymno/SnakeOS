export PATH := $(HOME)/opt/cross/bin:/usr/bin:/bin:$(PATH)

.PHONY: all clean fclean re

SRC_DIR := src
OBJ_DIR := obj
SRCS := \
	kernel.c	\
	arch/boot/boot.s \
	arch/gdt/gdtInit.s \
	arch/gdt/gdtReload.c \
	arch/hwio/ports.c \
	arch/boot/multiboot2/multiboot2_header.s \
	arch/boot/multiboot2/multiboot2.c \
	arch/memory/pmm.c \
	arch/memory/memoryMap.c \
	arch/graphics/framebuffer.c \
	lib/string.c \
	drivers/serial/serial.c 
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

build/SnakeOS.iso: SnakeOS.bin
	$(DIR_DUP)
	mv SnakeOS.bin build/iso/boot
	grub-mkrescue -o build/SnakeOS.iso build/iso

SnakeOS.bin: $(OBJS)
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
	$(RM) build/SnakeOS.iso

re:
	$(MAKE) fclean
	$(MAKE) all
