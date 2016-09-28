// 13.1g.cc

void f (int i, int j);
void f (int i, int j = 99);          // OK: redeclaration of f(int, int)
void f (int i = 88, int j);          // OK: redeclaration of f(int, int)
void f ();                           // OK: overloaded declaration of f

void prog ()
{
    f (1, 2);                        // OK: call f(int, int)

    // TODO: I fail to properly handle the next two calls because
    // I don't properly cascade the default arguments.  I am not
    // sure now I want to implement it, because I could either
    // go back and modify the params which originally didn't
    // have default arguments, or I could make a new type that
    // has the union of all the defaults seen so far ...
    //f (1);                           // OK: call f(int, int)
    //f ();                            // Error: f(int, int) or f()?
}
