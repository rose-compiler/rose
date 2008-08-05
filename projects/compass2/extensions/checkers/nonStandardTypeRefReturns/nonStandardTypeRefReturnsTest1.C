
//Your test file code goes here.

class a
{

};

typedef int b;

class c
{

};


a what ()
{
  return *(new a());
}

c& whatup ()
{
  c* kk = new c();
  return  *kk;
}

int main()
{
  a* d = new a();
  c* e = new c();
  b ff = 7;
  ff++;
};
