// t0183.cc
// multi-yield DeclSpecifier

// the declaration looks like a function prototype (which it is), 
// but also looks like a declaration of a templatized static member
// where "T&s" is the ctor argument

// fixed by using (the already ambiguous) InitDeclarator in
// TemplateDeclaration instead of splitting the two cases

template<class T>
int nothing(T &s);
