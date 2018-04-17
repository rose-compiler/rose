
// extern int getaddrinfo_a (int __mode, struct gaicb *__list[__restrict],int __ent, struct sigevent *__restrict __sig);
// extern int foo_a (int __mode ); // , struct gaicb *__list[__restrict],int __ent, struct sigevent *__restrict __sig);
// extern int foo_b (int __mode , struct gaicb *__list[__restrict]); // ,int __ent, struct sigevent *__restrict __sig);

// extern int foo_b (int __mode , int* __list[__restrict]);

// Defining the struct X before it's reference in the function parameter 
// list avoid infinit recursion in ROSE and demonstrates the other problem (in nginx).
struct X {};

// void foo_b (int __mode , struct X* __list[]);

// Note that we defined away "__restrict" in gcc_HEADERS/rose_edg_required_macros_and_functions.h to simplify this.
void foo_b (int __mode , struct X* __list[__restrict]);

