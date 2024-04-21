struct stat;

// syscall
int           exit(int)   __attribute__((noreturn));
int           fork();
int           execve(const char *, char **, char **);
int           wait(int *);

// ulib.c
size_t        strlen(const char *);
char *        strcpy(char *, const char *);
char *        strncpy(char *, const char *, size_t);
int           strcmp(const char *, const char *);
int           strncmp(const char *, const char *, size_t);
char *        strcat(char *, const char *);
void *        memset(void *, int, size_t);
void *        memmove(void *, const void *, size_t);
void *        memcpy(void *, const void *, size_t);
int           memcmp(const void *, const void *, size_t);
int           printf(const char *, ...);

