typedef void foobar_t(unsigned char key);

struct keyhandler {
    int diagnostic;
    union {
        foobar_t *fn;
        int xxx;
    } u;
};

static void dump_irqs(unsigned char key);

static struct keyhandler dump_irqs_keyhandler = {
//  .diagnostic = 1,
#if 0
    .u.fn = dump_irqs,
#else
    .u = { .fn = dump_irqs },
#endif
};

