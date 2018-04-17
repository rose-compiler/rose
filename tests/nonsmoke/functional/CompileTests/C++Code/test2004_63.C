// Exploring PIVOT
// PIVOT specifies the IR so that a statement is derived from an expression ???
// Their motivation is to handle the follwoing, how do we handle this?
//            if (Call* c = dynamic_cast<Call*>(e)) { /* ... */ }

class Call {};

int main()
   {
     Call* e;

     if (Call* c = dynamic_cast<Call*>(e)) { /* ... */ }

     return 0;
   }
