// c/k0011.c is the same file as k0011.cc, but it fails with -tr c_lang

// error: more than one ambiguous alternative succeeds

// originally found in package bzip2

// ERR-MATCH: (cannot evaluate.*as a template integer argument|cannot apply template args to non-template)

struct S{ int z; } *s;

int foo() {
    int x, y;

    s->z < 1 || 2 > (3);
    s->z < 1 && 2 > (3);

    x < 1 || y > (3);
    x < 1 && y > (3);

    x < 1 || 2 > (3);
    x < 1 && y > (3);
}
