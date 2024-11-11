#ifndef SCHEDULE_H__
#define SCHEDULE_H__

#define     RED       1
#define     BLACK     0
struct rbNode {
  struct proc *   p;
  uint8_t         color;
  struct rbNode*  left;
  struct rbNode*  right;
  struct rbNode*  parent;
};

void    rb_push_back(struct proc *);
void    rb_pop_front(struct proc *);
void    rb_init(void);

void    yield(void);
void    schedule(void);
#endif  //!SCHEDULE_H__
