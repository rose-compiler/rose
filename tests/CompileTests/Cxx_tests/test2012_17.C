
#if 0
// #include<stdlib.h>

struct A
   {
     static const void* a;
   };

const void* A::a = 0L;

const void* X = A::a;
#endif

class Car
   {
     public:
          int speed;
   };

int main()
   {
  // Example of pointer to member data.
     int Car::*pSpeed_null = 0L;

  // int Car::*pSpeed = &Car::speed;


     return 0;
   }
