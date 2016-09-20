// dC0026.c
// implicit int in K&R decl

void pack12 (p, n, last)
     register char *p;
     register /*implicit-int*/ n;
     int last;
{}
