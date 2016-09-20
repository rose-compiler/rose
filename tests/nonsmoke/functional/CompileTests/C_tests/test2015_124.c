typedef char bool_t;
struct cpu_user_regs;

typedef void keyhandler_fn_t(unsigned char key);
typedef void irq_keyhandler_fn_t(unsigned char key, struct cpu_user_regs *regs);

struct keyhandler {
    bool_t irq_callback;
    bool_t diagnostic;
    union {
        keyhandler_fn_t *fn;
        irq_keyhandler_fn_t *irq_fn;
    } u;
    char *desc;
};

static void dump_irqs(unsigned char key);

// Example using "dot" syntax
static struct keyhandler dump_irqs_keyhandler = {
    .diagnostic = 1,
    .u.fn = dump_irqs,
    .desc = "dump interrupt bindings"
};

// Example using alternative " = { }" syntax
static struct keyhandler temp_dump_irqs_keyhandler = {
    .diagnostic = 1,
    .u = { .fn = dump_irqs},
    .desc = "dump interrupt bindings"
};
