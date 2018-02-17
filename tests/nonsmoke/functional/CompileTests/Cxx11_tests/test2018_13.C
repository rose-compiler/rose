struct struct1 {};
struct struct2 {};

template < typename T >
class template_class1 {};

template < typename P, typename T >
using template_class2 = template_class1< T >;

template_class2< struct1, double > instance_class1();

template_class2< struct2, double > instance_class2();

