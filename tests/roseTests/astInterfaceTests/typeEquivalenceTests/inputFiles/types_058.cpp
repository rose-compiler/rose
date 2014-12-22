

typedef int foo;
typedef int bar;

void f(foo i);
void g(bar j);

void f(foo i){
}

void g(bar j){
}

int main(int argc, char**){

  foo a;
  bar b;

  f(b);
  g(a);

}
