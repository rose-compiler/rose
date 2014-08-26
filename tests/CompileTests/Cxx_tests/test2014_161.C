template <typename T>
class HistVarSet 
   {
     public:
          HistVarSet (int x) {}
#if 0
          void foo( HistVarSet<bool> flags );
#endif
   };

void foobar()
   {
     HistVarSet<bool> log_flags(7);
   }


