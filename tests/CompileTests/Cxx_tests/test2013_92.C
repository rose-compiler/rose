template<class type> class QStack_alt
   {
     public:
       // void push( const type *d );
       // void push( const type *d ) {};

       // When this function is built as an instantiation, it will cause the template type to get name qualification.
       // Where the template argument will be name qualified, this effects all uses of the template type, even in the
       // variable declaration: "QStack_alt<State> m_stateStack_alt;".  If this is not good enough then we need to 
       // keep maps of name qualifications so that they can be organized more specific to the declarations there the
       // types are being referenced (since types are shared but statements are not).  Note clear if we really need 
       // that level of complexity.
          void push() { type x; };
   };

struct TagFileParser
   {
     enum State { Invalid };

#if 0
  // Fortunately this is not legal code (good, since it would have made over qualification of some types in template arguments an error).
     struct TagFileParser { enum State { Invalid }; };
#endif

     void startMember()
        {
       // m_stateStack_alt.push(0L);
          m_stateStack_alt.push();
        }

  // Since types are shared and the template argument's name qualification is stored in
  // the type we have to allow some over qualification of the template type names here.
     QStack_alt<State> m_stateStack_alt;
   };


