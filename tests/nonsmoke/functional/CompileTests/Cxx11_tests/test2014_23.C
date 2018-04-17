
struct Builder
   {
     int makeObject();
   };

Builder builder;

// demonstrate decltype() with a meber function call expression.
decltype( builder.makeObject() ) x;
