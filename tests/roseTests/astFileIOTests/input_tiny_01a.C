// Isolated bug...
typedef struct Domain_s { } Domain_t;
int foo ( Domain_s *domain);
int foo ( Domain_t *domain);
