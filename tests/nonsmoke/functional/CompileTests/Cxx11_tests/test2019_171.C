struct X
   {
     X(int ii = 32);
   };

// BUG: unparsed as: struct X b[2] = {(1), ()};
X b[2] = { 1 };


