
class rational { };

class omni { };

class over
   {
     public:
          double d;
   };

void foobar()
   {
     double over::*pdv = &over::d;

  // Unparse as: class rational ::omni::*prm = reinterpret_cast < class rational ::omni::* >  (pdv);
  // Should be:        rational   omni::*prm = reinterpret_cast < rational omni::*>(pdv);
  // Could be: class rational omni::*prm = reinterpret_cast < class rational omni::* >  (pdv);
  // The issue is the extra "::" in both places.
     rational omni::*prm = reinterpret_cast<rational omni::*>(pdv);
   }

