#include <stddef.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>

static inline long syscall3(long num, long a0, long a1, long a2) {
    register long x10 asm("a0") = a0;
    register long x11 asm("a1") = a1;
    register long x12 asm("a2") = a2;
    register long x17 asm("a7") = num;
    asm volatile("ecall"
                 : "+r"(x10)
                 : "r"(x11), "r"(x12), "r"(x17)
                 : "memory");
    return x10;
}

struct host_file {
    struct __file file;
    int fd;
};

ssize_t _read(int fd, void *buf, size_t count);
ssize_t _write(int fd, const void *buf, size_t count);

static int host_put(char c, struct __file *f) {
    struct host_file *hf = (struct host_file *)f;
    return _write(hf->fd, &c, 1) == 1 ? (unsigned char)c : EOF;
}

static int host_get(struct __file *f) {
    struct host_file *hf = (struct host_file *)f;
    unsigned char c = 0;
    ssize_t n = _read(hf->fd, &c, 1);
    if (n <= 0)
        return _FDEV_EOF;
    return c;
}

static int host_flush(struct __file *f) {
    (void)f;
    return 0;
}

static struct host_file stdin_file = {
    .file = FDEV_SETUP_STREAM(NULL, host_get, host_flush, __SRD),
    .fd = 0,
};

static struct host_file stdout_file = {
    .file = FDEV_SETUP_STREAM(host_put, NULL, host_flush, __SWR),
    .fd = 1,
};

static struct host_file stderr_file = {
    .file = FDEV_SETUP_STREAM(host_put, NULL, host_flush, __SWR),
    .fd = 2,
};

FILE *const stdin = (FILE *)&stdin_file;
FILE *const stdout = (FILE *)&stdout_file;
FILE *const stderr = (FILE *)&stderr_file;

void _exit(int status) {
    syscall3(93, status, 0, 0);
    for (;;)
        ;
}

int _open(const char *path, int flags, int mode) {
    return (int)syscall3(1024, (long)path, flags, mode);
}

int _close(int fd) { return (int)syscall3(57, fd, 0, 0); }

ssize_t _read(int fd, void *buf, size_t count) {
    return (ssize_t)syscall3(63, fd, (long)buf, (long)count);
}

ssize_t _write(int fd, const void *buf, size_t count) {
    return (ssize_t)syscall3(64, fd, (long)buf, (long)count);
}

long _lseek(int fd, long offset, int whence) {
    (void)fd;
    (void)offset;
    (void)whence;
    return -1;
}

int _fstat(int fd, struct stat *st) {
    (void)fd;
    if (!st)
        return -1;
    st->st_mode = S_IFCHR;
    st->st_nlink = 1;
    st->st_size = 0;
    return 0;
}

int _isatty(int fd) {
    (void)fd;
    return 1;
}

void *_sbrk(ptrdiff_t incr) {
    extern char _end;
    static char *heap_end;
    char *prev;

    if (!heap_end)
        heap_end = &_end;

    prev = heap_end;
    heap_end += incr;
    return prev;
}


int open(const char *path, int flags, int mode) { return _open(path, flags, mode); }
int close(int fd) { return _close(fd); }
ssize_t read(int fd, void *buf, size_t count) { return _read(fd, buf, count); }
ssize_t write(int fd, const void *buf, size_t count) { return _write(fd, buf, count); }
long lseek(int fd, long offset, int whence) { return _lseek(fd, offset, whence); }
int fstat(int fd, struct stat *st) { return _fstat(fd, st); }
int isatty(int fd) { return _isatty(fd); }

int main(int argc, char **argv);

void _start(void) {
    register int argc asm("a0");
    register char **argv asm("a1");

    int rc = main(argc, argv);
    _exit(rc);
}
