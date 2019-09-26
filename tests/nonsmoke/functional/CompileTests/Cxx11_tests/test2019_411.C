
namespace A
   {
     struct S;
   }

// A::S* pointer;

struct X
   {
     A::S* get_vtable() const 
        {
       // Original code: return reinterpret_cast<detail::function::vtable_base*>(reinterpret_cast<std::size_t>(vtable) & ~static_cast<std::size_t>(0x01));
       // return reinterpret_cast<A::S*>(reinterpret_cast<std::size_t>(vtable) );
       // return reinterpret_cast<A::S*>(pointer);
 
      // This fails in the EDG/ROSE translation!
         return reinterpret_cast<A::S*>(0L);
        }
   };

