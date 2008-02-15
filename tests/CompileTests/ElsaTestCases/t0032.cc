// cc.in32
// anonymous unions

int main()
{
  union {
    unsigned ret;
    char c[4];
  };

  c[3] = 4;
  return ret;
}


int foo()
{
  // better not still be able to see 'ret' ..
  //ERROR(1): return ret;     // undeclared
}


struct nsStr {
  union {
    char*         mStr;
    short*        mUStr;
  };
};

void someFunc()
{
  nsStr s;
  s.mUStr;
}


char *anotherFunc()
{     
  //ERROR(2): return mStr;
}


