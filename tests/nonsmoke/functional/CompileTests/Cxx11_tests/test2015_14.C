
class XXX
   {
     void foobar();
   };

void XXX::foobar()
   {
     int dir = 0;

  // DQ (2/14/2015): This code is a problem for ROSE.
     auto hyp = [this,dir](int a_flux, int a_q)
        {
        };
   }
