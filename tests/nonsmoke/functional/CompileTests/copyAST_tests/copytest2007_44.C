
namespace XYZ 
   {
     template<typename T> class ABC {};

     typedef ABC<int>   integerType;
   }

XYZ::ABC<int> var;

#if 0
int main()
   {
  // std::templateInNamespace<double> A;
     std::templateInNamespace<int> B;

  // If this is commented out then the typedef above is not generated properly, specifically it is
  // generated as: typedef class templateInNamespace<actual template parameters> floatType;
  // std::templateInNamespace<float> C;

  // DQ (9/8/2004): This currently fails to generate a qualified name
  // std::floatType D;

  // This currently fails to qualify the name
  // typedef std::templateInNamespace<long> MyFavorite;
  // MyFavorite MyFavoriteObject;

     return 0;
   }
#endif
