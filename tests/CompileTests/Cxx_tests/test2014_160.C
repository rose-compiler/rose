
template <typename T>
class HistVarSet 
   {
     public:
          HistVarSet () {}
          void foo( HistVarSet<bool> flags );
   };

void foobar()
   {
     HistVarSet<bool> log_flags();
   }


