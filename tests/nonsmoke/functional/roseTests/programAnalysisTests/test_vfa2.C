#include<stdio.h>
class Animal {
 public:
   virtual void shout() {printf("Animal");}
};

class dog : public Animal{
  public:
  virtual void shout () {printf("dog");}
};
class terrier : public dog {
  public:
  virtual void shout () {printf("terrier");}
};
class yterrier : public terrier{
  public:
  virtual void shout () {printf("yterrier");}
};

int main(void) {

Animal **p, **q;
dog *x, d;
terrier *y;

y = new yterrier;
x = &d;
p = (Animal **)&x;
q = p;
*p = y;

x->shout();

return 0;

}

