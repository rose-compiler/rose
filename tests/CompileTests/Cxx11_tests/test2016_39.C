// Liao, 6/24/2016
// Extracted from a RAJA header

struct seq_exec{};
template<typename POLICY, typename IdxI=int, typename TI, typename BODY>
void forallN(TI const &is_i, BODY const &body){
}

double* a;
int main()
{
  int is; 
  // instantiate it once
  forallN<seq_exec>(is, [=] (int i) {
      a[i] = i + 0.1;
      });
  return 0;
}
