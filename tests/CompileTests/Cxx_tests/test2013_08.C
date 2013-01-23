
// This is the template
template<class _CharT> struct char_traits { };

// This is the template specialization.
template<> struct char_traits<char>
   {
  // This fails when trying to identify the template instantiations pointer back to the template member function.
  // The question is should we have such a template declaration for the member function.

  // I think this is the canonical example of where a template declaration 
  // might be unavailable for the instantiated template member function.

  // static void assign() { };
#if 1
     void assign() {};
#else
     void assign();
#endif
   };



