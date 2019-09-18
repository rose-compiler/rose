class rational { };

class omni { };

void foobar()
   {
  // Unparse as: class rational ::omni::*prm = 0L;
  // Should be:        rational   omni::*prm = 0L;
  // Could be: class rational omni::*prm = 0L;
  // The issue is the extra "::" in both places.
     rational omni::*prm;
   }
