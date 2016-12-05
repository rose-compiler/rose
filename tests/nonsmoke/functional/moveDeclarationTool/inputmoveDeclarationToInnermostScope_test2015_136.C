void foo(int* i);

int foobar()
   {
     int abc  = 0 ;

     while ( ! abc ) {

          int xyz ;

          if (!true) {

               abc = 1;
               break;
          }

          if ( !true ) {

              foo(&xyz) ;

          } 

         else if ( !true ) {

              foo(&xyz) ;
         } 
      }

     return 0;
   }

