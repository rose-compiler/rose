struct struct1_s 
   {
     int member1;
   };

template < typename kernel_type >
inline void for_all(kernel_type&& kernel )
   {
   }

static void func1(struct1_s* parm3)
   {
     for_all< > ( [=] ()
        {
          int local1 = parm3->member1;
        } );
   }
