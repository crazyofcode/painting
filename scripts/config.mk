
TOOLPREFIX = riscv64-linux-gnu-
QEMU = qemu-system-riscv64

OBJCOPY = $(TOOLPREFIX)objcopy
OBJDUMP = $(TOOLPREFIX)objdump


LIB_PATH := $(HOME)/Desktop/os6828/paintingOS/include
SBI-QEMU := ./opensbi/sbi-qemu

QEMUOPTS = -machine virt -cpu rv64 -kernel $T/kernel -m 1G -smp $(CPUS) -nographic
QEMUOPTS += -bios $(SBI-QEMU)
QEMUOPTS += -drive file=fs.img,if=none,format=raw,id=x0
QEMUOPTS += -device virtio-blk-device,drive=x0,bus=virtio-mmio-bus.0

CC = $(TOOLPREFIX)gcc
AS = $(TOOLPREFIX)gas
LD = $(TOOLPREFIX)ld
OBJCOPY = $(TOOLPREFIX)objcopy
OBJDUMP = $(TOOLPREFIX)objdump

CFLAGS = -Wall -Werror -O -fno-omit-frame-pointer -ggdb -gdwarf-2
CFLAGS += -MD
CFLAGS += -mcmodel=medany
CFLAGS += -ffreestanding -fno-common -nostdlib -mno-relax
CFLAGS += -I$(LIB_PATH)
CFLAGS += $(shell $(CC) -fno-stack-protector -E -x c /dev/null >/dev/null 2>&1 && echo -fno-stack-protector)

