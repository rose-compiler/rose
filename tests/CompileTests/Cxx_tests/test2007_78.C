namespace X
{
   namespace Y
   {
      class B;
   }
}

namespace Y {
   using namespace X::Y;
   B* b1;
   void B();
   class B* b2;
}


