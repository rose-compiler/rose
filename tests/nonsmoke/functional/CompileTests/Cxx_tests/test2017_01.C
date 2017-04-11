// This is a C++11 feature for which the test code has been moved to the Cxx11_tests directory.

#if 0
namespace Myspace {
        template<typename T>
        class A {
                T component;
        };
    template<typename V>
        using Alias = Myspace::A<V>;
}


using Myspace::A;  //THIS WORKS.
using Myspace::Alias; //THIS CAUSES EDG/ROSE TO FAIL.

#endif
