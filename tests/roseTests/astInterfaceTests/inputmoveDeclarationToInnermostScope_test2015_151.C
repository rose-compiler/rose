// This test code is testing both only formating, and
// the correctness of the -rose:merge_decl_assign option.

int yy;

void foobar()
   {
     int aa;
     int bb;
     int cc;
     for (int i = 0 ; i < 42; i++ ) 
        {
          aa = i;
          bb = i;
          cc = i;
          yy = 42;
        }
   }


