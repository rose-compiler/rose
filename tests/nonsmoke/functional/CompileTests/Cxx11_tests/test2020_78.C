// ROSE-2587

class Class_3 {
  public:
//NEEDED: const:      
    void func_2(void) const;
};

class Class_2: public Class_3 {
  public:
    void func_3();
// NEEDED:
    void func_2(void) const;
};

void Class_2::func_3(){
// NEEDED:
  Class_3::func_2();
}

template<typename t_t_parm_2>
class Class_1 {
  public:
    template<typename t_t_parm_3>
    Class_1(t_t_parm_3 && parm_4);
};

Class_1<int> func_4();

// gets the following warning:
// identityTranslator[35482] 2.95606s Rose[WARN ]: Note: Not clear how this happens: classChain.size() > 1 size = 2 
// Note: In AstTests::runAllTests(): command line option used to skip AST consistancy tests 

