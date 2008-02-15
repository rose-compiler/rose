
// This test code demonstrates a problem where the Sg_File_Info object is 
// not set correctly (value is the default value, which is never correct).


class X
   {
     public:
          X();

          int x;
          int y[4];
   };

X::X():x(),y() {}

