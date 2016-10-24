class X {};
class Y : public X
   {
     public:
         Y();
         Y(double x);
   };

// Make the converted types more different.
// void* xPtr1 = reinterpret_cast<Y*>(yPtr);
void* xPtr1 = reinterpret_cast<Y*>(42);
