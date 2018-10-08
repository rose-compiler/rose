template <typename T1> class map {};

namespace namespace1 
   {
     template <typename T2> using t_Class1 = map<int>;
   }

namespace1::t_Class1<double> var1;
