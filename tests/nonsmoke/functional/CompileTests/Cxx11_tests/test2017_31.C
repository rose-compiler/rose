
// DQ: copy of test2017_30.C but made as simple as possible to still domonstrate the error.

template <typename EXEC_POLICY_T, typename LOOP_BODY>
void forall(LOOP_BODY loop_body) { }

struct switcher_exec{};

void foobar()
   {
     double* a = 0L;

     forall<switcher_exec> ( [=] (int i) 
        {
          a;
        });
   }

