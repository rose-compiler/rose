
namespace std
   {
     class type_info {};
   }

int A;

const std::type_info& ti1 = typeid(A);
