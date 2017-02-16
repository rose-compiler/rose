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

