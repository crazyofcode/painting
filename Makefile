K = kernel
U = user
T = build
CPUS := 2


include ./scripts/config.mk


OBJS = \
			 $T/entry.o\
			 $T/main.o\
			 $T/console.o\
			 $T/proc.o\
			 $T/printf.o\
			 $T/spinlock.o\
			 $T/string.o\

$T/%.o: $K/%.S
	$(shell mkdir -p $(T))
	$(CC) $(ASFLAGS) -c $< -o $@

$T/%.o: $K/%.c
	$(CC) $(CFLAGS) -c $< -o $@

$T/kernel: $(OBJS) $K/kernel.ld $U/initcode
	$(LD) $(LDFLAGS) -T $K/kernel.ld -o $T/kernel $(OBJS)
	$(OBJDUMP) -S $T/kernel > $T/kernel.asm
	$(OBJDUMP) -t $T/kernel | sed '1,/SYMBOL TABLE/d; s/ .* / /; /^$$/d' > $T/kernel.sym

$U/initcode: $U/initcode.S
	$(CC) $(CFLAGS) -I$(LIB_PATH) -march=rv64g -nostdinc -I. -Ikernel -c $U/initcode.S -o $T/initcode.o
	$(LD) $(LDFLAGS) -N -e start -Ttext 0 -o $T/initcode.out $T/initcode.o
	$(OBJCOPY) -S -O binary $T/initcode.out $U/initcode
	$(OBJDUMP) -S $T/initcode.o > $T/initcode.asm

qemu: $T/kernel
	$(QEMU)	$(QEMUOPTS)

clean:
	rm -rf ./build
PYTHON: clean
