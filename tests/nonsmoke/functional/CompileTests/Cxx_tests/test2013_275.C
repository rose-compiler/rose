class A

{

public:

   double dummy ;

   int NofGroups() { return 5 ; }

} ;

 

class B

{

public:

   double dummy ;

} ;

 

class Foo

{

public:

   Foo(A * rgl) ;

   int m_NofGroups ;

   B **m_comps ;

} ;

 

Foo::Foo(A * rgl)

   : m_NofGroups(rgl->NofGroups())

   , m_comps(new B * [m_NofGroups])

{

}

 
