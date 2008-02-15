// t0002.c
// problem with enums?

typedef int wchar_t;

int mbtowc (int y, wchar_t *   __pwc);

enum { A, B = A, };
