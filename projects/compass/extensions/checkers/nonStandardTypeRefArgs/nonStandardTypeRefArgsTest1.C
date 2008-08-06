


typedef int age;

typedef struct duo
{
  int first;
  int second;
} duo;

struct animal
{
  int mine;
  int yours;
};

class dynamic
{
  int a;
};

int f3a (int jeff, duo d)
{
  return 0;
}
int f3b (int jeff, const duo d)
{
  return 0;
}
int f3c (duo const d)
{
  return 0;
}
int f3r (duo const &d)
{
  return 0;
}

int f2 (animal a)
{
  return 0;
}

int f2r (int l, animal &a, animal b, duo d)
{
  return 0;
}

int f1 (dynamic d)
{
  return 0;
}

int f4 (int j)
{
  return 0;
}

int f5 (age a)
{
  return 0;
}

int main()
{
  
  return 0;
}
