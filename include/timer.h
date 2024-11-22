#ifndef TIMER_H__
#define TIMER_H__

#define INTERVAL    1000000
#define CLOCK_PER_USEC  10

void        clock_init();
void        clock_intr();
uint64_t    get_timestamp();
uint64_t    get_time();

#endif //!TIMER_H__
