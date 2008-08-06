class Foo{


};

typedef const Foo* Foo2;

int main(){
 Foo2 x ;

 Foo* y;
 y = (Foo*) x;

};
