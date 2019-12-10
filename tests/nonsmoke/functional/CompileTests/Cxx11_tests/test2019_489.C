// ROSE-2086

template<typename t_t_parm_2>
class Class_3
{
public:
    Class_3()
    { }

    Class_3(t_t_parm_2* parm_3)
    { }

    t_t_parm_2*
    func_2()
    { return 0L; }
};

class Class_2
   {
     public:
          Class_2(const Class_3<char>& parm_1, bool parm_2);
   };

Class_3<char> local_3;

class Class_1
   {
     public: 
          inline void func_1()
             {
            // Bug: unparses to:
            // class Class_2 *local_1 = new Class_2 (local_3 . func_2()),false;
               class Class_2 *local_1 = new Class_2 (local_3 . func_2(),false);
             }
   };
