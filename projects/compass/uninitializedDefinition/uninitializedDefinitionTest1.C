int file_scope_noinit; // OK, static
int file_scope_init = 23;

void use(int *, ...);
 
struct foo
{
    int struct_member; // OK, class member
};
 
void f(int n)
{
    int func_scope_init = n;
    int func_scope_noinit; // NOT OK, checker should warn
    extern int func_scope_extern; // OK, extern
    auto int func_scope_auto; // NOT OK, checker should warn
 
    struct foo st; // OK, class type (has constructor)
 
    static int func_scope_static; // OK, static

    // use the variables so the frontend doesn't complain...
    use(&func_scope_init, &func_scope_noinit, &func_scope_auto, &st,
            &func_scope_static);
}

void f_noncompliant()
{
    int x; // not OK, no initializer
    use(&x);
}

struct bar {
    int member; // OK, class member
};

void f_compliant(int n)
{
    int x = n; // OK, initializer present
    static int y; // OK, static
    struct bar st; // OK, class type (has constructor)
    extern int not_here; // OK, extern

    use(&x, &y, &st);
}
