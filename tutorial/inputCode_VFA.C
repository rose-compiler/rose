class Animal {
 public:
   virtual void shout() {}
};

class dog : public Animal{
  public:
  virtual void shout () {}
};
class terrier : public dog {
  public:
  virtual void shout () {}
};
class yterrier : public terrier{
  public:
  virtual void shout () {}
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

