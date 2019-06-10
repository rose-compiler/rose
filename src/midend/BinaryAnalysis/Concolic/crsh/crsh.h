
#ifndef CRSH_H
#define CRSH_H

// crush
// - to press between opposing bodies so as to break, compress, or injure

void crsh_opendb(const char*);
void crsh_closedb();

void crsh_suite(const char*);
void crsh_suite_end();

void crsh_test(const char*);
void crsh_test_end();

void crsh_cmdlarg(const char*);
void crsh_invoke(const char*);
void crsh_envvar(const char*, const char*);

#endif /* CRSH_H */
