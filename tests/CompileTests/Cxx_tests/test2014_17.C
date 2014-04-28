namespace sc_dt
{
class sc_lv_base;

class sc_lv_base
   {
     public:
          sc_lv_base();
          sc_lv_base(int x);
   };
}




namespace sc_dt
{

template <int W> class sc_lv;

template <int W>
class sc_lv : public sc_lv_base
   {
     public:
          sc_lv() : sc_lv_base( W ) {}
   };
}


namespace std
{
template<class T> class vector
   {
   };
}

   
namespace sc_core 
{
class sc_process_b;

template <int W>
class sc_lv_resolve
   {
     public:
       // static void resolve(sc_dt::sc_lv<W>&);
          static void resolve(sc_dt::sc_lv<W>&,std::vector<sc_dt::sc_lv<W>*>&);
   };
}
