class B
   {
     public:
          enum color { R, O, Y };
   };

struct D : B
   {
     typedef char color;
     struct T 
        {
       // BUG: this is unparsed as: "enum color c;" instead of "B::color c;"
          B::color c; 
        };
   };

