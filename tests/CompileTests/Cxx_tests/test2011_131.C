// If a transformation of this is done then 
class A 
{
   public:
      template<class T>
      class B
      {
      }; 
};

   

void foo()
   {
     A::B<int> x;
   }
