#ifndef _TAU_FUJITSU_H_
#define _TAU_FUJITSU_H_

#include <sys/types.h>

extern "C" {
extern int getdtablesize (void);

extern int getopt(int, char *const *, const char *);
extern char *optarg;
extern int optind, opterr, optopt;

extern int strcasecmp(const char *s1, const char *s2);
int strncasecmp(const char *s1, const char *s2, size_t n);
}

#endif /* _TAU_FUJITSU_H_ */
