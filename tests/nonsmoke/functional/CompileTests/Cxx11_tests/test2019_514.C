int main(int argc, char *argv[])
   {
     struct S { char c; };
     typedef char S::*volatile PM;
  // typedef PM PM_t; 

#if 0
     PM_t PM_v = 0; 
     PM_t *PM_p = &PM_v; 
     PM_p->PM_t::~PM_t();
#endif

     return 42;
   }
