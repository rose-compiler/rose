namespace N
   {
     class color;
   }

// When the definition is in the typedef then we still need the name qualification.
typedef class N::color {} new_type;
