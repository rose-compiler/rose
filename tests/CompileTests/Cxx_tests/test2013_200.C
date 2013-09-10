class Descriptor {};

struct Symbol 
   {
     enum Type { NULL_SYMBOL, MESSAGE, FIELD, ENUM, ENUM_VALUE, SERVICE, METHOD, PACKAGE };

     Type type;

     union 
        {
          const Descriptor* descriptor;
        };

     inline Symbol() : type(NULL_SYMBOL) { descriptor = 0L; }

     inline Symbol(const class Descriptor *value)
        {
          (this) -> type = MESSAGE;
          (this) -> descriptor = value;
        }

   };
