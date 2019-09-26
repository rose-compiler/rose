
namespace A
   {
     struct S;
   }

A::S* pointer;

void foobar()
   {
  // Original code: pointer = reinterpret_cast<A::S*>(pointer);
  // Unparsed to:   pointer = (reinterpret_cast < struct S * >  (pointer));
     pointer = reinterpret_cast<A::S*>(pointer);
   }



