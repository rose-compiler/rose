
struct Builder
   {
     int makeObject();
   };

Builder builder;

// demonstrate decltype() with a member function call expression.
decltype( builder.makeObject() ) x;
