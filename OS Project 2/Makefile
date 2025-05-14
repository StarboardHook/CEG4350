# Directories
SRC_DIR = ./src
INCLUDE_DIR = ./include
ASM_DIR = ./asm
BUILD_DIR = ./build

# Compiler and flags
CC = gcc
AS = as
LD = ld
NASM = nasm
CFLAGS = -m32 -fno-pie -ffreestanding -Wall -Wextra -I$(INCLUDE_DIR)

# Source files
C_SOURCES = $(wildcard $(SRC_DIR)/*.c)
ASM_SOURCES = $(filter-out $(ASM_DIR)/kernel_entry.asm, $(wildcard $(ASM_DIR)/*.asm))
KERNEL_ENTRY_ASM = $(ASM_DIR)/kernel_entry.asm
INTERRUPT_ASM = $(ASM_DIR)/interrupt.asm
BOOTLOADER_ASM = $(ASM_DIR)/bootloader.asm
FAT_ASM = $(ASM_DIR)/fat.asm
ROOT_DIR_ASM = $(ASM_DIR)/root_dir.asm

# Object files
C_OBJECTS = $(patsubst $(SRC_DIR)/%.c, $(BUILD_DIR)/%.o, $(C_SOURCES))
ASM_OBJECTS = $(patsubst $(ASM_DIR)/%.asm, $(BUILD_DIR)/%.o, $(ASM_SOURCES))
KERNEL_ENTRY_OBJ = $(BUILD_DIR)/kernel_entry.o
INTERRUPT_OBJ = $(BUILD_DIR)/interrupt.o

# Binary files
BOOTLOADER_BIN = $(BUILD_DIR)/bootloader.bin
FAT_BIN = $(BUILD_DIR)/fat.bin
ROOT_DIR_BIN = $(BUILD_DIR)/root_dir.bin
KERNEL_BIN = $(BUILD_DIR)/kernel.bin

# OS Image
OS_IMG = $(BUILD_DIR)/os.img

# Targets
all: $(OS_IMG)

$(OS_IMG): $(BOOTLOADER_BIN) $(FAT_BIN) $(ROOT_DIR_BIN) $(KERNEL_BIN)
	cat $(BOOTLOADER_BIN) $(FAT_BIN) $(ROOT_DIR_BIN) $(KERNEL_BIN) > $(OS_IMG)

$(KERNEL_BIN): $(KERNEL_ENTRY_OBJ) $(C_OBJECTS) $(INTERRUPT_OBJ)
	$(LD) -m elf_i386 -s -o $@ -Ttext 0x1000 $^ --oformat binary

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

$(ROOT_DIR_BIN): $(ROOT_DIR_ASM)
	$(NASM) $< -f bin -o $@

$(FAT_BIN): $(FAT_ASM)
	$(NASM) $< -f bin -o $@

$(BOOTLOADER_BIN): $(BOOTLOADER_ASM)
	$(NASM) $< -f bin -o $@

$(KERNEL_ENTRY_OBJ): $(KERNEL_ENTRY_ASM)
	$(NASM) $< -f elf -o $@

$(INTERRUPT_OBJ): $(INTERRUPT_ASM)
	$(NASM) $< -f elf -o $@

clean:
	rm -rf $(BUILD_DIR)/*