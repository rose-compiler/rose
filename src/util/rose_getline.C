#include <algorithm>
#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <cerrno>
#include "rose_getline.h"

static int
get_next_char(FILE *stream) {
    return fgetc(stream);
}

static int
get_next_char(std::istream &stream) {
    return stream.get();
}

template<class Stream>
ssize_t
rose_getline_impl(char **lineptr, size_t *n, Stream &stream) {
    assert(lineptr);
    assert(n);
    assert(stream);
    assert((0==n && NULL==lineptr) || (0!=n && NULL!=lineptr));

    size_t nread=0;
    while (1) {
        if (nread >= *n) {
            *n = std::max((size_t)256, *n*2);
            *lineptr = (char*)realloc(*lineptr, *n);
            if (!*lineptr) {
                *n = 0;
                return -1;
            }
        }

        if (nread>0 && '\n'==(*lineptr)[nread-1]) {
            (*lineptr)[nread] = '\0';
            return nread;
        }

        errno = 0;
        int c = get_next_char(stream);
        if (c<0) {
            (*lineptr)[nread] = '\0';
            return nread>0 ? nread : -1;
        } else {
            (*lineptr)[nread++] = (char)c;
        }
    }
}
    


ssize_t
rose_getline(char **lineptr, size_t *n, FILE *stream) {
    return rose_getline_impl(lineptr, n, stream);
}

ssize_t
rose_getline(char **lineptr, size_t *n, std::istream &stream) {
    return rose_getline_impl(lineptr, n, stream);
}

std::string
rose_getline(FILE *stream) {
    struct Resources {
        char *buffer;
        size_t bufsz;

        Resources()
            : buffer(NULL), bufsz(0) {}

        ~Resources() {
            if (buffer)
                free(buffer);
        }
    } r;

    ssize_t nread = rose_getline(&r.buffer, &r.bufsz, stream);
    return nread > 0 ? std::string(r.buffer, r.buffer + nread) : std::string();
}

std::string
rose_getline(std::istream &stream) {
    struct Resources {
        char *buffer;
        size_t bufsz;

        Resources()
            : buffer(NULL), bufsz(0) {}

        ~Resources() {
            if (buffer)
                free(buffer);
        }
    } r;

    ssize_t nread = rose_getline(&r.buffer, &r.bufsz, stream);
    return nread > 0 ? std::string(r.buffer, r.buffer + nread) : std::string();
}
