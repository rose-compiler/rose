template<class point_rep, class duration_rep>
class period 
   {
     public:
          period();

     private:
          point_rep last_;
   };

#define BUG 1

// DQ: The use of the preinitialization list for the template argument reference causes the bug.
template<class point_rep, class duration_rep>
inline
period<point_rep,duration_rep>::period() 
#if BUG
   : last_(duration_rep::unit())
#endif
   {
#if !BUG
     last_ = duration_rep::unit();
#endif
   }



