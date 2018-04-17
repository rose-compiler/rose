// Example showing use of friends and testing use of namespace qualification (should not be used)
struct A 
{
      int x;
      A(int x): x(x) {}
      friend bool operator==(const A& a, const A& b) 
      {
         return a.x == b.x;
      }
      friend bool eq(const A& a, const A& b) 
      {
         return a.x == b.x;
      }
};

int main(int, char**) 
{
   A a1(3), a2(4);

   bool b = a1 == a2;

// No namespace qualification should be used here
   bool c = eq(a1,a2);
   return 0;
}
