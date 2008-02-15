void foo1();
void foo2();
void foo4();
void foo3()
{
}
void foo5(){};

void foo1(){
  foo2();
  foo3();
}

void foo2(){
  foo3();
  foo4();
}

int main(int argc,char ** argv){
  foo1();
  foo2();
  foo3();
  return 0;
}
