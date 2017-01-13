// t0545.cc
// overload resolution of address-of-template-func in E_new ctor args

template <class T>
void f(T);

void g(int);
void g(int,int);

struct A {
  A(void (*)(int));
};

void foo()
{
  new A(&g);
  new A(&f<int>);
}



// --------- more complete example --------
class OpaqueValueData {};

class OpaqueValue {
public:
  OpaqueValue (OpaqueValueData * value);
};

template < typename Type >
class OVDPtrImpl : public OpaqueValueData {
public:
  OVDPtrImpl(Type *src, void (*destructor) (Type *));
};

template < typename Type >
void defaultDestructorFunc (Type * data);

template < typename Type >
class Ptr : public OpaqueValue {
public:
  Ptr (Type * value)
    : OpaqueValue(new OVDPtrImpl<Type>(value, &defaultDestructorFunc<Type>))
  {}
};

void bar()
{
  Ptr<int> q((int*)0);
}
