#include "start.h"

.section .text
.global entry
entry:
  mv a1, sp
  j start
