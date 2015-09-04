
void foo(int len,int condition)
{
  for (int i = 0; i < len; ++i) {
  }
  if (condition) {
    for (int i = 0; i < len; ++i) {
    }
  }
}
// A tricky case of if-stmt, 
// move to two bodies, trigger another round of moving: iterative moving
#if 1

void bar(bool allow)
{
  for (int i = 0; i < 10; ++i) {
  }
  if (allow) {
    for (int i = 0; i < 4; ++i) {
    }
  }
   else {
    for (int i = 0; i < 4; ++i) {
    }
  }
}
#endif
// if statement issue
// move to two bodies, trigger another round of moving
#if 1

void foo2(bool allow,int update,int update2)
{
  int j;
  if (update > 0) {
    for (int i = 0; i < 12; ++i) {
    }
    for (int gp = 0; gp < 8; ++gp) {
      if (update2 > 0) {
        for (int k = 0; k < 4; ++k) {
          if (allow) {
            int i;
            for ((i = 0 , j = 0); i < 10; (++i , ++j)) {
            }
          }
           else {
            int i;
            for ((i = 0 , j = 0); i < 10; (++i , ++j)) {
            }
          }
        }
      }
    }
  }
}
#endif 

class foo 
{
  

  public: inline foo(double val) : m_a(val)
{
  }
  void f1(bool allow,int update,int length,double *b);
  private: double m_a;
}
;

void foo::f1(bool allow,int update,int length,double *b)
{
  if (allow) {
    for (int iz = 0; iz < length; ++iz) {
      int dens;
      dens = ((this) -> m_a / b[iz]);
    }
  }
   else {
    if (update > 0) {
      for (int iz = 0; iz < length; ++iz) {
        int dens;
        dens = ((this) -> m_a / b[iz]);
      }
    }
     else {
      for (int iz = 0; iz < length; ++iz) {
        int dens;
        dens = ((this) -> m_a / b[iz]);
      }
    }
  }
}
