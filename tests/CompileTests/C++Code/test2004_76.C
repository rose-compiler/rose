template<class U>
class A
   {
         U x;

     public:
         template<class T> void g(int y, T u) 
            {
              y = y+u;
              x+=u;
              
            }
         void f(int y)
         {
            x=x+y;
            
         }
         
   };

#if 0
// define the following specialization (else this code demonstrates an error within ROSE)
inline void A < int > ::f(int y)
{
  (this) -> x = (this) -> x + y;
}
#endif

// explicit template instantiation directive
// template void A<int>::g<int>(int y, int u);
// template void A<int>::f(int y);

int main()
   {
     A<int> a;
     int x,y;
     a.g<int>(x,y);   

  // DQ (9/5/2005): Commented out to allow tests with AST testing in place. It used to pass just fine!
  // This causes the attribute test to fail because a node is shared (SgTemplateInstantiationMemberFunctionDecl, I think)
     a.f(x);

     return 0;
   }

