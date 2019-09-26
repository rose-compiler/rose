// ROSE-1906: (C++03) Declaration of specialized virtual method of template classes

//  Two cases: inside namespace or in global scope. Inside template reproduce the issue unparsing STL headers.

//  2nd case: (might be an unrelated issue)

//      The specialization is not unparsed. The unparsed file compile but it could cause errors at link time.

            template<typename T>
            struct A {
              virtual void foo();
            };
            template<>
            void A<char>::foo();

