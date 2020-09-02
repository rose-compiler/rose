// ROSE-2505 (Tristan)

// NOTE: this failes in EDG 5.0, and needs to be tested in EDG 6.0.

template <typename t_type2>
class Class1 {
public:
   t_type2* func3();
   // const REQUIRED:
   void func1(const Class1<t_type2>& values);
};

template <typename t_type2>
void Class1<t_type2>::func1(const Class1<t_type2>& values) 
{
   this->unreal_func2(*values.func3());
}

// but gets this error in /collab/usr/global/tools/rose/toss_3_x86_64_ib/rose-master-0.9.13.53-gcc-4.9.3/bin/identityTranslator:
// "/usr/WS2/charles/code/ROSE/rose-reynolds12-automation/scripts/lc/KULL/testing/
//          ROSE-86.cc", line 12: error: the object has type qualifiers that are
//          not compatible with the member function "Class1<t_type2>::func3"
//            object type is: const Class1<t_type2>
//     this->unreal_func2(*values.func3());
