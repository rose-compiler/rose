

struct es__stream {};

typedef struct es__stream *estream_t;

#define ES__RESTRICT __restrict__

// This demonstrats that __restrict__ can be applied to a typedef.
void es_freopen (estream_t ES__RESTRICT stream);

