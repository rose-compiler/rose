// ROSE-43
// ROSE-1311

namespace local {
typedef signed char int8;
class Class1 {
public:
    static Class1 int8();
    static long func1();
};
long Class1::func1() {
   return sizeof(local::int8);
}
}

