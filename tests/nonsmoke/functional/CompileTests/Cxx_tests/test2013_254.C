
int handle_sigchld()
   {
     int status;

  // This is related to a problem for C, but demonstrating the error in this wa is only 
  // possible for C++ (simple declaration in switch only allowed in C++).
     switch (int x = 5)
        {
       // This body without a case statment will cause an empty block to be generated with this
       // block not in the switch body and instead listed as a block after the switch statement.
          status = x;
        }
   }

