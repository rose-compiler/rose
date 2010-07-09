
class AClass 
{
  

  public: inline AClass()
{
  }

  

  inline AClass(int x,int y)
{
  }

  

  inline int &foo(int &x)
{
    return x;
  }

  

  inline int bar(int x)
{
    return x * 2;
  }

}

;

int globalFunc(int a,int b)
{
  return a + b;
}


int main(int argc,char **argv)
{
  int a = 3;
  int &b = a;
  class AClass aObject;
  int __temp0__;
  int __temp1__;
  int __temp2__;
//Function calls in both the initializer and the test,
//with declaration in the initializer
  for (int i = (__temp0__ = (a | b) , ((__temp1__ = (a & b) , globalFunc(__temp0__,__temp1__)))); ((__temp2__ = aObject. bar (a) , aObject. bar (__temp2__))) == 0; ) {
    break; 
  }
  int __temp3__;
  int __temp4__;
  int __temp5__;
//Function calls in both the initializer and the test,
  for ((__temp3__ = (a | b) , ((__temp4__ = (a & b) , globalFunc(__temp3__,__temp4__)))); ((__temp5__ = aObject. bar (a) , aObject. bar (__temp5__))) == 0; ) {
    break; 
  }
  int __temp6__;
  int __temp7__;
  int __temp8__;
//Function calls in both the initializer and the test,
//with multiple declaration in the initializer
  for (int i = (__temp6__ = (a | b) , ((__temp7__ = (a & b) , globalFunc(__temp6__,__temp7__)))), j; ((__temp8__ = aObject. bar (a) , aObject. bar (__temp8__))) == 0; ) 
    break; 
  int __temp9__;
//Function in for-loop increment expression
  for (; ; (__temp9__ = aObject. bar (a) , globalFunc(__temp9__,b))) {
    break; 
  }
  int *__temp10__;
//Constructor initialization inside the for loop
  for (class AClass localA(3,3); ((__temp10__ = &aObject. foo (a) , aObject. bar ( *__temp10__))); ) 
    break; 
  int *__temp11__;
//Aggregate initializer inside the for loop
  for (int hello[] = {aObject. bar (a), (3)}; ((__temp11__ = &aObject. foo (a) , aObject. bar ( *__temp11__))); ) {
    hello[0] = 7;
    break; 
  }
  int *__temp12__;
  int *__temp13__;
//More complex aggregate initializer inside the for loop
  for (int hello[][2UL] = {{(1), ((__temp12__ = &aObject. foo (a) , aObject. bar ( *__temp12__)))}, {(1), (2)}}; ((__temp13__ = &aObject. foo (a) , aObject. bar ( *__temp13__))); ) {
    (hello[0])[0] = 7;
    break; 
  }
  int *__temp14__;
//Moving the increment expression
  for (; ; (__temp14__ = &aObject. foo (a) , aObject. bar ( *__temp14__))) {
    globalFunc(1,2);
  }
  int __temp15__;
//A variable declared in the init expression is accessed in the test expression,
  for (int i = 12; ((__temp15__ = aObject. bar (i) , globalFunc(__temp15__,3))); ) {
  }
  return 0;
}

