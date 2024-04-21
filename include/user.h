struct stat;

// syscall
int           exit(int)   __atribute__((noreturn));
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
void *        memset(char *, int, size_t);
void *        memmove(char *, const char *, size_t);
void *        memcpy(char *, const char *, size_t);
int           memcmp(const char *, const char *, size_t);


