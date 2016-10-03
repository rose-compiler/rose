
int handle_sigchld()
   {
     int status;

     switch (5)
        {
       // This body without a case statment will cause an empty block to be generated with this
       // block not in the switch body and instead listed as a block after the switch statement.
       // NOTE: if there is a case statement then this code will work fine. So this is an issue
       // that is a bit contrived and not yet seen in real code that I am aware of.  Still I 
       // would like to fix it when we implement better switch statement handling.
          status = 8;
        }
   }

