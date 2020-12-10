// ROSE-1302

class Class1 {

  void func3();

  void func2();

};

 

template < typename t_t_parm1,

           typename t_t_parm2 >

inline void t_func1( t_t_parm2&& parm1 ) {

}

 

struct struct1 {

};

 

void Class1::func3() {

    t_func1< struct1 >( [=] {

           this->func2();

    } );

}

 
