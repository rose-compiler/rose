namespace Myspace {
        template<typename T>
        class A {
                T component;
        };

 // This is called an alias template (C++11 specific).
    template<typename V>
        using Alias = Myspace::A<V>;

 // Fortunately a typedef can't exist with a template typedef.
 // typedef int Alias;
}


// using Myspace::A;  //THIS WORKS.
using Myspace::Alias; //THIS CAUSES EDG/ROSE TO FAIL.

// This will work if the "using Myspace::Alias;" is fixed.
// Alias<int> x;

// Note: this will be unparsed as: Myspace::A< int  > x;
// Myspace::Alias<int> x;
