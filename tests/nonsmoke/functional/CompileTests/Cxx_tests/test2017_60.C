
template<class point_rep, class duration_rep>
class period 
   {
     public:
       // typedef point_rep point_type;
       // typedef duration_rep duration_type;

          period(point_rep end_point);

     private:
          point_rep last_;
   };

template<class point_rep, class duration_rep>
inline
period<point_rep,duration_rep>::period(point_rep end_point) 
// : last_(end_point - duration_rep::unit())
   : last_(duration_rep::unit())
   {
  // duration_rep::unit();
   }






