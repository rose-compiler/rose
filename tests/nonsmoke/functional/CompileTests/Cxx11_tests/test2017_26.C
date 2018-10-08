
struct seq_exec{};

template<typename POLICY, typename IdxI=int, typename TI, typename BODY>
void forallN(TI const &is_i, BODY const &body){}

struct seq_reduce {};

template<typename REDUCE_POLICY_T, typename T> class ReduceMin;

template <typename T>
class ReduceMin<seq_reduce, T>
   {
     public:
          explicit ReduceMin(T init_val)
             {
             }

          operator T()
             {
               return m_reduced_val;
             }

          ReduceMin<seq_reduce, T> min(T val) const
             {
               return *this ;
             }

     private:                                                                                                                       
          T m_reduced_val;                                                                                                            
   };

void foobar()
   {
     int is; 
  // instantiate it once
     ReduceMin<seq_reduce, float> dthydroLoc (float(1.0e+20));

  // Error: unparses as:
  // ::forallN< seq_exec  , int  , int { public: ReduceMin< seq_reduce  , float  > dthydroLoc;} > (is, [=] (int i)
     forallN<seq_exec>(is, [=] (int i) 
        {
          dthydroLoc.min(0.0);
       });
   }
