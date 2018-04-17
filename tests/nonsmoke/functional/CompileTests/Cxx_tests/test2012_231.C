void foo ()
   {
     class A {} x1;

  // BUG: the TRUE statment is output in the function scope and the true branch is empty.
  // Once this was fixed the problem was that both true and false branches had to have seperate declarations.
  // a SgBasicBlock was inserted to support such declarations having seperate scopes.
     if (true)
          class A {} x3;
       else
          class A {} x3;
   }
