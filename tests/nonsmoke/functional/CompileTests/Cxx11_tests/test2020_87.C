// ROSE-2505 (test2020_80.C) but made for g++ to fail.

template <typename t_type2>
class Class1 {
public:
   t_type2* func3();
   void func1(const Class1<t_type2>& values);
};

template <typename t_type2>
void Class1<t_type2>::func1(const Class1<t_type2>& values) 
{
   this->unreal_func2(*values.func3());
}

int foo() {
  Class1<int> p,q;
  p.func1(q);
}

