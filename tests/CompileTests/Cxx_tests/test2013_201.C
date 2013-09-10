class Descriptor {};

struct Symbol 
   {
  // This works if the union is named, but fails to be unparsed if the union is un-named.
  // As an un-named union this is not a type declatation as much as it is a variable declaration of an un-named variable.
  // union X
     union 
        {
          const Descriptor* descriptor;
        };
   };
