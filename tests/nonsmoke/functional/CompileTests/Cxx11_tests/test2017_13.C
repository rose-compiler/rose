
namespace RAJA
   {
     struct seq_exec{};

     template<typename POLICY, typename IdxI=int, typename TI, typename BODY>
     void forallN(TI const &is_i, BODY const &body){}
   }

void foobar()
   {
     int is; 
     RAJA::forallN<RAJA::seq_exec>(is, [=] (int i) { 42; });
   }
