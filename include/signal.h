union __sigaction_u {
  void    (*__sa_handler)(int);
  void    (*__sa_sigaction)(int, struct __siginfo *,
      void *);
};

struct sigaction {
  union __sigaction_u __sigaction_u;    // signal handler
  sigset_t    sa_mask;        // signal mask to apply
  int         sa_flags;       // see signal options below
}
