namespace Myspace {
        template<typename T>
        class A {
                T component;
        };

 // This is called an alias template (C++11 specific).
    template<typename V>
        using Alias = Myspace::A<V>;
}


using Myspace::A;  //THIS WORKS.
using Myspace::Alias; //THIS CAUSES EDG/ROSE TO FAIL.

