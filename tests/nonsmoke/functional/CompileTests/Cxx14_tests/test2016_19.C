
template<typename Func>
void LambdaTest(Func f) 
   {
     f(10);
   }

int main()
   {
     int test = 5;
     LambdaTest([&](int a) { test += a; });

     return 0;
   }

