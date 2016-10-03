// Liao, 6/24/2016
// Extracted from a RAJA header

namespace RAJA{

struct seq_exec{};
template<typename POLICY, typename IdxI=int, typename TI, typename BODY>
void forallN(TI const &is_i, BODY const &body){
}

struct seq_reduce {};

template<typename REDUCE_POLICY_T,
         typename T>
class ReduceMin;

template <typename T>
class ReduceMin<seq_reduce, T>
{
public:
   explicit ReduceMin(T init_val)
   {
      m_is_copy = false;

      m_reduced_val = init_val;

//      m_myID = getCPUReductionId();

//      m_blockdata = getCPUReductionMemBlock(m_myID);

//      m_blockdata[0] = init_val;
   }

   ReduceMin( const ReduceMin<seq_reduce, T>& other )
   {
      *this = other;
      m_is_copy = true;
   }

   ~ReduceMin<seq_reduce, T>()
   {
//      if (!m_is_copy) {
//         releaseCPUReductionId(m_myID);
//      }
   }

   operator T()
   {
      //m_reduced_val = (((static_cast<T>(m_blockdata[0])) < (m_reduced_val)) ? (static_cast<T>(m_blockdata[0])) : (m_reduced_val));

      return m_reduced_val;
   }

   ReduceMin<seq_reduce, T> min(T val) const
   {
      //m_blockdata[0] = (((val) < (static_cast<T>(m_blockdata[0]))) ? (val) : (static_cast<T>(m_blockdata[0])));
      return *this ;
   }

private:                                                                                                                       
   ReduceMin<seq_reduce, T>();                                                                                                 
                                                                                                                               
   bool m_is_copy;                                                                                                             
   int m_myID;                                                                                                                 
                                                                                                                               
   T m_reduced_val;                                                                                                            
                                                                                                                               
//   CPUReductionBlockDataType* m_blockdata;                                                                                     
} ;             

}

double* a;
float t;
int main()
{
  int is; 
  // instantiate it once
  RAJA::ReduceMin<RAJA::seq_reduce, float> dthydroLoc (float(1.0e+20));
  RAJA::forallN<RAJA::seq_exec>(is, [=] (int i) {
      float t = a[i];
      a[i] = i + 0.1;
      dthydroLoc.min(t);
      });

#if 0
  double sum =0.0; 
  forallN<seq_exec>(is, [=,&sum] (int i) {
      sum = a[i] + i + 0.1;
      });
#endif
  return 0;
}
