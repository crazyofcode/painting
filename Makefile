K = kernel
U = user
T = build
CPUS := 2


include ./scripts/config.mk


OBJS = \
			 $T/entry.o\
			 $T/main.o\
			 $T/console.o\
			 $T/swtch.o\
			 $T/proc.o\
			 $T/printf.o\
			 $T/spinlock.o\
			 $T/string.o\
			 $T/kalloc.o\
			 $T/bio.o\
			 $T/buddy.o\
			 $T/timer.o\
			 $T/vm.o\
			 $T/sleeplock.o\
			 $T/trampoline.o\
			 $T/trap.o\
			 $T/plic.o\
			 $T/syscall.o\
			 $T/virtio.o\
			 $T/kernelvec.o\

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

ULIB = \
				$U/ulib.o\
				$U/printf.o\
				# $U/usys.o\
				# $U/umalloc.o\

_%: %.o $(ULIB)
	$(LD) $(LDFLAGS) -T $U/user.ld -o $@ $^
	$(OBJDUMP) -S $@ > $*.asm
	$(OBJDUMP) -t $@ | sed '1,/SYMBOL TABLE/d; s/ .* / /; /^$$/d' > $*.sym

$U/usys.S : $U/usys.pl
	perl $U/usys.pl > $U/usys.S

$U/usys.o : $U/usys.S
	$(CC) $(CFLAGS) -c -o $U/usys.o $U/usys.S

# Prevent deletion of intermediate files, e.g. cat.o, after first build, so
# that disk image changes after first build are persistent until clean.  More
# details:
# http://www.gnu.org/software/make/manual/html_node/Chained-Rules.html
.PRECIOUS: %.o

UPROCES = \
					$U/_init\

qemu: $T/kernel
	$(QEMU)	$(QEMUOPTS)

qemu-gdb: $T/kernel .gdbinit fs.img
	@echo "*** Now run 'gdb' in another window." 1>&2
	$(QEMU) $(QEMUOPTS) -S $(QEMUGDB)
clean:
	rm -rf ./build ./user/initcode
PYTHON: clean
