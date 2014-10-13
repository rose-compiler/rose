

// This function demonstrates the new return type syntax.
template <typename Builder>
auto
makeAndProcessObject (const Builder& builder) -> decltype( builder.makeObject() )
   {
     auto val = builder.makeObject();
  // do stuff with val
     return val;
   }

