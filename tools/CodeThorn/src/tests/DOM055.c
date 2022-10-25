#include <string.h>

void *realloc( void *ptr, size_t new_size );
	
#define DO_REALLOC(basevar, sizevar, needed_size, type)	do {		\
  long DR_needed_size = (needed_size);					\
  long DR_newsize = 0;							\
  while ((sizevar) < (DR_needed_size)) {				\
    DR_newsize = sizevar << 1;						\
    if (DR_newsize < 16)						\
      DR_newsize = 16;							\
    (sizevar) = DR_newsize;						\
  }									\
  if (DR_newsize)							\
    basevar = realloc (basevar, DR_newsize * sizeof (type));		\
} while (0)

#define GROW(g, append_size) do {                                       \
  struct growable *G_ = g;                                              \
  DO_REALLOC (G_->base, G_->size, G_->tail + append_size, char);        \
} while (0)

/* Return the tail position of the string. */
#define TAIL(r) ((r)->base + (r)->tail)

/* Move the tail position by APPEND_COUNT characters. */
#define TAIL_INCR(r, append_count) ((r)->tail += append_count)

struct growable {
  char *base;
  int size;
  int tail;
};

static void
append_string (const char *str, struct growable *dest)
{
  int l = strlen (str);
  GROW (dest, l);
  memcpy (TAIL (dest), str, l);
  TAIL_INCR (dest, l);
}

int main() {
  const char s[]="teststring";
  char b[20];
  struct growable d;
  d.base=b;
  d.size=19;
  d.tail=strlen(s);

  append_string(s,&d);
}
