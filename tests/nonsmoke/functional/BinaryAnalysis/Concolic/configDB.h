
#ifndef _CONFIG_DB_H
#define _CONFIG_DB_H 1

#if defined(ROSE_HAVE_SQLITE3) && defined(__linux)
#define TEST_CONCOLICDB 1
#else
#define TEST_CONCOLICDB 0
#endif /* w/ sqlite && linux */

#endif /* _CONFIG_DB_H */
