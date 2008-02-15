// expressions

//typedef struct page { int x; } page;

typedef struct pte_t { int y; } pte_t;

// this is not ambiguous because "()" is not
// a valid ctor-initializer (there must be arguments there)
int get_pte_fast();

int foo ( )
{
  pte_t * page = (pte_t *) get_pte_fast();
}
