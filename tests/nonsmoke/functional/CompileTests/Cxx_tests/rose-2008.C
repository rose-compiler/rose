template <typename t_t_parm_1>
class Class_1;

template <typename t_t_parm_2>
class Class_2 {
    void func_2() {
      member_1; // Error happens when converting the type for this field reference
    }

    // no issue if only class:
    //   is the nonreal (anonymous) class stored in the nonreal scope of the templated typedef ???
    typedef class { } struct_1;

    Class_1<struct_1> member_1;
};
