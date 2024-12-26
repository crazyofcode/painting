#include "start.h"
#include "lib.h"

void start(long *p) {
	int argc = p[0];
	char **argv = (void *)(p + 1);
	int ret = main(argc, argv);
	exit(ret);
}

