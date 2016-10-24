// cc.in58
// use a typedef as a qualifier

class A {
public:
  static int x;
};

typedef A B;

int main()
{
  return B::x;
}
