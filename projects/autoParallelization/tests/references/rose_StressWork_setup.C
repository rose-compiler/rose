// functions within a namespace
// 
#include "omp.h" 
namespace abc
{

class StressWorkBaseLoopAlgorithm 
{
private: double *m_dele;
double *m_volold;
double *m_volnew;
int m_namix;
void setup();
}
;

void StressWorkBaseLoopAlgorithm::setup()
{
//  const int N = m_Domain->m_namix;
const int N = (this) -> m_namix;
(this) -> m_dele = (new double [N]);
#if 0
#endif
(this) -> m_volold = (new double [N]);
(this) -> m_volnew = (new double [N]);
//#pragma omp parallel for schedule(static)
#pragma omp parallel for
for (int i = 0; i <= N - 1; i += 1) {
  (this) -> m_dele[i] = 0.0;
  (this) -> m_volold[i] = 2.0;
  (this) -> m_volnew[i] = 3.5;
}
#if 0
#endif
}
}
