// t0255.cc
// demonstrate two ambiguities that require a merge() after
// I allowed "template" in TemplateId ...

template <class T>
class A {};

template <class T>
A<T>& foo(T)
{ }


template class A<char>;
template class A<int>;

// requires merge of SimpleDeclaration
// this is gnu-specific it seems, and is tested in gnu/g0002.cc
//extern template
//A<char>& foo(char);

// requires merge of Declaration
template
A<int>& foo(int);
