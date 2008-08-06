class Foo{


};

int main(){
 

 const Foo* x = new Foo();

 Foo* y;
 y = const_cast<Foo*>(x);

};
