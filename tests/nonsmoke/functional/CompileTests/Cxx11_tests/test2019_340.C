// ROSE-1906: (C++03) Declaration of specialized virtual method of template classes

//  Two cases: inside namespace or in global scope. Inside template reproduce the issue unparsing STL headers.

//  1st case:

//      Unparsed code will not compile

            namespace N {
              template<typename T>
              struct A {
                virtual void foo();
              };
              template<>
              void A<char>::foo();
            }


