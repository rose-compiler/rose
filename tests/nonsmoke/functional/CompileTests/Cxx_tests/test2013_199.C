#define NULL 0L

class Descriptor {};
class FieldDescriptor {};

struct Symbol 
   {
     enum Type { NULL_SYMBOL, MESSAGE, FIELD, ENUM, ENUM_VALUE, SERVICE, METHOD, PACKAGE };
     Type type;
     union 
        {
          const Descriptor* descriptor;
          const FieldDescriptor* field_descriptor;
        };

     inline Symbol() : type(NULL_SYMBOL) { descriptor = NULL; }
  // inline bool IsNull() const { return type == NULL_SYMBOL; }
  // inline bool IsType() const { return type == MESSAGE || type == ENUM; }
  // inline bool IsAggregate() const { return type == MESSAGE || type == PACKAGE || type == ENUM || type == SERVICE; }

#define CONSTRUCTOR(TYPE, TYPE_CONSTANT, FIELD)  \
  inline explicit Symbol(const TYPE* value) {    \
    type = TYPE_CONSTANT;                        \
    this->FIELD = value;                         \
  }

     CONSTRUCTOR(Descriptor         , MESSAGE   , descriptor             )
  // CONSTRUCTOR(FieldDescriptor    , FIELD     , field_descriptor       )

#undef CONSTRUCTOR

   };
