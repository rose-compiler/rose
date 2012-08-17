class Animal{
private:
    int type;
public:
    void myType();
};

class Dog : public Animal{
private:
    int type;
public:
    void change();

};


int *p,*k;
int g=0;

void bar()
{
  int x,c;
  int *q;
  q=&g;
  #pragma rose p: Aliases:{ }{}\
  g: Aliases:{ }{}\
  x: Aliases:{ }{}\
  c: Aliases:{ }{}\
  q: Aliases:{ g }{}\
  p: Aliases:{ }{}

  p=q;
  #pragma rose p: Aliases:{ g }{}\
  g: Aliases:{ }{}\
  x: Aliases:{ }{}\
  c: Aliases:{ }{}\
  q: Aliases:{ g }{}\
  p: Aliases:{ }{}

  if (1) // flag is only read here, not written! 
  {
    q = &x;
    #pragma rose p: Aliases:{ g }{}\
    g: Aliases:{ }{}\
    x: Aliases:{ }{}\
    c: Aliases:{ }{}\
    q: Aliases:{ x }{}\
    p: Aliases:{ }{}
  }
  else  
  { int *p ;
    p = &c;
    
    #pragma rose p: Aliases:{ g }{}\
    g: Aliases:{ }{}\
    x: Aliases:{ }{}\
    c: Aliases:{ }{}\
    q: Aliases:{ g }{}\
    p: Aliases:{ c }{}
  }


  #pragma rose p: Aliases:{ g }{}\
    g: Aliases:{ }{}\
    x: Aliases:{ }{}\
    c: Aliases:{ }{}\
    q: Aliases:{ g x }{}\
    p: Aliases:{ c }{}
}


int main()
{
 Animal *an = new Animal();
 //HERE THE ALIAS IS AN EXPRESSION FOR NEW_EXP. SINCE A NEW ADDRESS MAY BE GENERATED WITH EACH EXECUTION LEAVING THE PRAGMA HALFWAY
 #pragma rose an: Aliases:{ __expression_
 
 an = new Dog();
 //HERE THE ALIAS IS AN EXPRESSION FOR NEW_EXP. SINCE A NEW ADDRESS MAY BE GENERATED WITH EACH EXECUTION LEAVING THE PRAGMA HALFWAY
 #pragma rose an: Aliases:{ __expression_
 
 bar();
 //HERE THE ALIAS IS AN EXPRESSION FOR NEW_EXP. SINCE A NEW ADDRESS MAY BE GENERATED WITH EACH EXECUTION LEAVING THE PRAGMA HALFWAY
 #pragma rose an: Aliases:{ __expression_

 return 0;
}

