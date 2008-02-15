// casting function pointer with throw() to/from no throw

// allowed by 8.3.5p4

// originally found in package abiword

// error: cannot convert argument type `int (*&)() throw()' to parameter 1
// type `int (*)()'

// ERR-MATCH: cannot convert argument type `.*throw\(\)'

int bar1 (int (*func) ());
int bar2 (int (*func) () throw());

int foo1 ();
int foo2 () throw ();

int main() {
    // bar(foo);

    int (*func1) ()           = 0;
    int (*func2) () throw()   = 0;

    func1 = func2;        // ok, func2 has fewer behaviors
    func2 = func1;        // questionable, but legal (8.3.5p4)

    bar1(func1);
    bar1(func2);          // ok, func2 has fewer behaviors

    bar2(func1);          // questionable
    bar2(func2);

    bar1(foo1);
    bar1(foo2);           // ok, foo2 has fewer behaviors

    bar2(foo1);           // questionable
    bar2(foo2);
}
