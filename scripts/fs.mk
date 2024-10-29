PROJ_PATH = ~/Desktop/painting/kernel
FS_SRC := $(wildcard $(PROJ_PATH)/fs/*.c) $(wildcard $(PROJ_PATH)/fs/*/*.c)
FS_OBJ := $(FS_SRC:.c=.o)

$T/$(FS_OBJ): $(FS_SRC)
	$(CC) $(CFLAGS) -c $< -o $@

