template<class duration_rep>
class period 
   {
     public:
          period();

     private:
          int last_;
   };

#define BUG 1

// DQ: The use of the preinitialization list for the template argument reference causes the bug.
template<class duration_rep>
inline
period<duration_rep>::period() 
#if BUG
   : last_(duration_rep::unit())
#endif
   {
#if !BUG
     last_ = duration_rep::unit();
#endif
   }

