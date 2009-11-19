void __mkbvvar(void *val, char *name);

int test(int x) {
     int y, z;
     __mkbvvar(&y, "y");
     __mkbvvar(&z, "z");
     assert(y+z == z+y);
     return 0;
}
