K = kernel
U = user
CPUS := 4


-include ./scripts/config.mk
$(shell mkdir -p $(T))
-include ./scripts/fs.mk

OBJS = \
			 $T/entry.o\
			 $T/main.o\
			 $T/printf.o\
			 $T/spinlock.o\
			 $T/console.o\
			 $T/proc.o\
			 $T/trampoline.o\
			 $T/string.o\
			 $T/pm.o\
			 $T/list.o\
			 $T/dtb.o\
			 $T/vm.o\
			 $T/trap.o\
			 $T/kernelvec.o\
			 $T/plic.o\
			 $T/virt.o\
			 $T/swtch.o\
			 $T/schedule.o\

$T/%.o: $K/%.S
	$(CC) $(ASFLAGS) -c $< -o $@

$T/%.o: $K/%.c
	$(CC) $(CFLAGS) -c $< -o $@

$T/kernel: $(OBJS) $K/kernel.ld $T/$(FS_OBJ) # $U/initcode
	$(LD) $(LDFLAGS) -T $K/kernel.ld -o $T/kernel $(OBJS)
	$(OBJDUMP) -S $T/kernel > $T/kernel.asm
	$(OBJDUMP) -t $T/kernel | sed '1,/SYMBOL TABLE/d; s/ .* / /; /^$$/d' > $T/kernel.sym

# $U/initcode: $U/initcode.S
# 	$(CC) $(CFLAGS) -I$(LIB_PATH) -march=rv64g -nostdinc -I. -Ikernel -c $U/initcode.S -o $T/initcode.o
# 	$(LD) $(LDFLAGS) -N -e start -Ttext 0 -o $T/initcode.out $T/initcode.o
# 	$(OBJCOPY) -S -O binary $T/initcode.out $U/initcode
# 	$(OBJDUMP) -S $T/initcode.o > $T/initcode.asm

qemu: $T/kernel
	$(QEMU)	$(QEMUOPTS)

qemu-gdb: $T/kernel .gdbinit fs.img
	@echo "*** Now run 'gdb' in another window." 1>&2
	$(QEMU) $(QEMUOPTS) -S $(QEMUGDB)
clean:
	rm -rf ./build
PYTHON: clean

