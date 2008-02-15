// error: more than one ambiguous alternative succeeds

// originally found in package bzip2

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
