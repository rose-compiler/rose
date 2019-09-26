// Folding expressions

template<typename... Args>
  bool f(Args... args) { 
    return (true + ... + args); // OK
  } 

#if 0
template<typename... Args>
  bool f(Args... args) { 
    return (args && ... && args); // error: both operands contain unexpanded parameter packs
  }
#endif
