class B
   {
     public:
       // enum color { R, O, Y };
   };

struct D : B
   {
     int color;
     enum color { R, O, Y };
     struct T 
        {
       // BUG: this is unparsed as: "enum color c;" instead of "B::color c;"
          enum color c; 
        };
   };

