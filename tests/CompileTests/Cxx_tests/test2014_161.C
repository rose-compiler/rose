template <typename T>
class HistVarSet 
   {
     public:
#if 1
       // HistVarSet (int x) {}
          HistVarSet (int x);
#endif
#if 1
          void foo( HistVarSet<bool> flags );
       // HistVarSet<bool> flags;
#endif
   };

HistVarSet<bool> log_flags(7);

#if 1
void foobar()
   {
     HistVarSet<bool> log_flags(7);
   }
#endif

