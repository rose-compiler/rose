// struct Domain_s;

typedef struct Domain_s {
   int a;
} Domain_t ;


// These are both a prototype to the same function (below).
extern int foo (struct Domain_s *domain);

// These are both a prototype to the same function (above).
int foo (Domain_t *domain);


// Only one definition is allowed since they are the same function.
#if 0
int foo ( struct Domain_s *domain ) {}
#endif

// Only one definition is allowed since they are the same function.
int foo ( Domain_t *domain ) {}

