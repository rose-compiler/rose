// These all pass for C++, but fail for C showing that this
// is a style of writing code that is specific to C++.
void foo()
   {
  // These statements allow a declaration ("simple declaration") in the predicate 
  // and this is why they are scopes.
     if (int i = 0);
     if (int i = 0) {}

  // The related subject applied to the "for" statement:
     for (int i = 0; int j = 8; j++);
  // And different from:
     for (int i = 0, j = 8; i < 7; j++);
  // or:
     for (int i = 0, j = 8; ; j++);

     switch (int i = 8);
     switch (int i = 8) {}

     while (int i = 8);
     while (int i = 8) {}

  // The do ... while does not allow a declaration in the predicate.
  // because the declaration would not be "considered" visible in the 
  // body from a language design point of view (though C could make 
  // it so it does not). And so a declaration in the predicate is not
  // allowed for the "do while" statement, where as it is for the "while"
  // statement.  The ROSE IR observes this by makeing only the "while"
  // IR node a scope and the "do while" as statement.
     int j = 0;
     do { int x = 1; } while (j <= 8);
  // do { int x = 1; } while (int j = 8);  // This is an error

  // Note that these examples show another more standard reason why these
  // need to be scopes.  Namely the body can be a declaration statement 
  // and the scope of the declaration should rightly be the if, switch, 
  // or while statement instead of anything else.
     if (int i = 0) int x = 7;
     switch (int i = 8) int x = 8;
     while (int i = 8) int x = 9;

  // Note that this is not allowed, showing that the 2nd "i" declaration 
  // must be in the "if" scope and not the implied body of the "if".
  // I used to get these finer details wrong until I decided to fix this
  // up properly several years ago.
  // if (int i = 0) int i = 7;      // This is an error
  // switch (int i = 8) int i = 8;  // This is an error
  // while (int i = 8) int i = 9;   // This is an error

  // Note that this is also allowed...(so the IR has an SgStatement* for the body instead of a SgBasicBlock).
     do int x = 1; while (j <= 8);
  // But the following is not...
  // do int x = 1; while (int x = 8);
   }
