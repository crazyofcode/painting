TAR = target
TOOLPREFIX = riscv64-linux-gnu-

CC = $(TOOLPREFIX)gcc
AS = $(TOOLPREFIX)gas
LD = $(TOOLPREFIX)ld
OBJCOPY = $(TOOLPREFIX)objcopy
OBJDUMP = $(TOOLPREFIX)objdump

CFLAGS = -Wall -Werror -O -fno-omit-frame-pointer -ggdb -gdwarf-2
CFLAGS += -MD
CFLAGS += -mcmodel=medany
CFLAGS += -ffreestanding -fno-common -nostdlib -mno-relax
CFLAGS += $(shell $(CC) -fno-stack-protector -E -x c /dev/null >/dev/null 2>&1 && echo -fno-stack-protector)

LDFLAGS = -z max-page-size=4096

$(shell mkdir -p $(TAR))

UPROGS = \
						$(TAR)/init

ULIB = $(TAR)/lib.o

$(TAR)/%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

$(TAR)/%: $(TAR)/%.o $(ULIB)
	$(LD) $(LDFLAGS) -T ./user.ld -o $@ $^
	$(OBJDUMP) -S $@ > $*.asm
	$(OBJDUMP) -t $@ \
		| sed '1,/SYMBOL TABLE/d; s/ .* / /; /^$$/d' > $*.sym

fs.img: $(UPROGS)
	@if [ ! -f "../fs.img" ]; then \
		echo "making fs image..."; \
		dd if=/dev/zero of=fs.img bs=512k count=512; \
		mkfs.vfat -s 8 -F 32 fs.img; fi
	@sudo mount ../fs.img ../mnt
	@for file in $(UPROGS); do \
		sudo cp $$file ../mnt/`basename $$file`;\
		echo "Copied `basename $$file` to $(MNT)"; \
	done
	@sudo umount ../mnt

clean: 
	rm -f *.asm *.sym \
	rm -rf $(TAR)
