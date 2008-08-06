//Should not give any error message. It is not a const cast.

class Foo{


};

typedef Foo Foo2;
int main(){
 

 Foo2* x = new Foo();

 Foo* y;
 y = const_cast<Foo*>(x);

};
