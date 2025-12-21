export PATH := $(HOME)/opt/cross/bin:/usr/bin:/bin:$(PATH)

.PHONY: all clean fclean re

SRC_DIR := src
OBJ_DIR := obj
C_SRCS := $(shell find $(SRC_DIR) -name "*.c")
ASM_SRCS := $(shell find $(SRC_DIR) -name "*.asm")
OBJS := $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(filter %.c, $(C_SRCS))) \
        $(patsubst $(SRC_DIR)/%.asm, $(OBJ_DIR)/%.o, $(filter %.asm, $(ASM_SRCS)))

AS := nasm
ASFLAGS := -felf32
CC := i686-elf-gcc
CFLAGS := -std=gnu99 -ffreestanding -O2 -Wall -Wextra \
          -Isrc/include \
          -Isrc/kernel/include \
          -Isrc/arch/x86/include
LINKER := i686-elf-gcc
LDFLAGS := -ffreestanding -T linker.ld -O2 -nostdlib -lgcc \
          -Isrc/include \
          -Isrc/kernel/include \
          -Isrc/arch/x86/include

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

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.asm
	$(DIR_DUP)
	$(AS) $(ASFLAGS) -o $@ $<

clean: 
	$(RM) -r obj

fclean:
	$(MAKE) clean
	$(RM) build/SnakeOS.iso

re:
	$(MAKE) fclean
	$(MAKE) all
