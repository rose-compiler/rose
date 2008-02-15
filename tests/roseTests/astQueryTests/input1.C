
class Bar{
public:
  Bar(int x, char y){};
};

int a;

void foo()
   {
     float b;
     Bar x(1,'x');
   }

class hiddenClassObjectTag
   {
     long c;
   } hiddenClassObject1;

typedef class hiddenClassTypeTag
   {
     double d;
   } hiddenClassType;

hiddenClassType hiddenClassObject2;
