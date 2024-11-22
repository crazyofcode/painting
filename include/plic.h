#ifndef PLIC_H__
#define PLIC_H__

void        plicinit(void);
void        plicinithart(uint64_t);
int         plic_claim(void);
void        plic_complete(int);
#endif //PLIC_H__
