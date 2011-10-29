#include<stdio.h>
class Animal {
 protected:
   char name[30];
   int age;
 public:
   Animal(char *_name) {
//      strcpy(name,_name);
  }
   virtual void shout() {printf("Animal");}
   ~Animal() {};
};

class Silky {
        public:
        Silky () {}
        virtual void shout2 () {printf("silky");}
};
class dog : public Animal{
  public:
  dog(char *_name) : Animal(_name) {}
  virtual void walk () {}
  virtual void shout () {printf("dog");}
   ~dog() {};

};
class terrier : public Silky, public dog {
  public:
  terrier(char *_name) : Silky(), dog(_name) {}
  virtual void shout () {printf("terrier");}
};

class cat : public Animal{
  public:
  cat(char *_name) : Animal(_name) {}
  virtual void shout () {printf("cat");}
   ~cat() {};
};


Animal * createDog() {
//Animal * x= new terrier("pepper2");
int a =1;
if(a) {
return  new terrier("pepper2");
} else 
return  new cat("yyye");
}

int main(void) {

//    Animal *a = new terrier("pepper");
    Animal tt("xxx");
    Animal *a;
    //a = &tt;
    a = createDog();
    Animal *b = new cat("tesla");
    a->shout();
    return 0;
}

