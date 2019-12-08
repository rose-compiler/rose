// ROSE-2076
class common_name_1
{
public:
    common_name_1();
};
namespace Namespace_1
{
    typedef common_name_1 typedef_1;

    struct Struct_1
    {
        Struct_1();
        typedef_1 common_name_1;
    }
    ;

 // Bug: name qualification should no be used here.
 // Struct_1::Struct_1() : Namespace_1::common_name_1()
    Struct_1::Struct_1() : common_name_1()
    {
    }
}
