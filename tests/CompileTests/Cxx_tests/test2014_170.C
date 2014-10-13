struct B {};
struct X : B 
   {
#if 0
     operator B&() { return *this; };
#endif
     operator B();
   };

B foo (X abc)
   {
     return abc.operator B();
   }

#if 0
int main()
   {
     X x;
     B& b1 = x;                  // does not call X::operatorB&()
     B& b2 = static_cast<B&>(x); // does not call X::operatorB&
     B& b3 = x.operator B&();    // calls X::operator&
   }
#endif
