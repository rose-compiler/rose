/*
   Bug contributed by: Chadd Williams <chadd@cs.umd.edu>
 */

/*******BEGIN CODE*******************/

  /* the following struct is taken from 
   * linux in /usr/include/bits/waitstatus.h
   */
struct containsUnNamed
      {
        unsigned int __w_termsig:7; /* Terminating signal.  */
        unsigned int __w_coredump:1; /* Set if dumped core.  */
        unsigned int __w_retcode:8; /* Return code if exited normally.  */


        /* HERE IS THE ERROR. 
         * if you name this field the assert
         * should not fail
         */ 
        unsigned int :16;

      };


int foo(){
        return 42;
}

int main(int argc, char**argv){
        foo();
}

/***********END CODE********************/

