// cc.in18
// class forward decls

class Foo;

int main()
{
  Foo *x;
  
  //ERROR(1): x->y;   // incomplete type

}

void f(Foo *) {}

class Foo {
public:
  int x;
};

// this is ok because it's an overloading
void f(Foo *, int) {}

// since Foo from above is same Foo as here, we'll
// get an error about duplicate definitions
//ERROR(2): void f(Foo *) {}     // duplicate definition


