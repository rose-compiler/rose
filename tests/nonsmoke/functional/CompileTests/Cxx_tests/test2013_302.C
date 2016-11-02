// This code will not compile (since the template functions are equivalent).
// See: http://stackoverflow.com/questions/17313649/how-can-i-distinguish-overloads-of-templates-with-non-type-parameters

template<int module>
    void template_const(int &a,int & b){
            a = a & module;
            b = b % module;
    }

    template<bool x>
    void template_const(int &a,int & b){
            int w;
            if (x){
                    w = 123;
            }
            else w = 512;
            a = a & w;
            b = b % w;
    }

void foo()
   {
     int a,b;

#if 0
  // DQ (8/12/2013): These template functions can be defined but not called (because they can't be disambiguated based on different integer kinds).
     template_const<true>(a,b)
     template_const<123>(a,b)
#endif
   }

