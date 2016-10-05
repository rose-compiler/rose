// This is an example of a bug (un-named enum in a class)
class X
   {
     public:
       // An empty enum is an error at present
          enum {};
       // enum { abcdefghijklmnop };
   };

