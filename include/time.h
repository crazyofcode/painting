#ifndef __TIME_H_
#define __TIME_H_

struct timespec {
    uint64  tv_sec;     /*seconds*/
    uint64  tv_nsec;    /*nanoseconds*/
};

uint64  mktime(const uint   year,  const uint mon, const int day, const int hour, const int min, const int sec);

void       CurrentTimeSpec(struct timespec *ts);

#endif      // !__TIME_H_