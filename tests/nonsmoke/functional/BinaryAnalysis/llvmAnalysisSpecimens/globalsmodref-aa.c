/* OPT: -globalsmodref-aa -aa-eval -print-alias-sets */

int a, b, c;

void _start() {
    c = a + b;
}
