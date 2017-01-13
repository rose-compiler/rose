// A test case extracted from loop suite
// Representing unparsing error after loop normalization
// ((LoopAlgorithm *)(this)) -> m_Domain -> m_kmax 
// complains about protected member is accessed illegally. 
#include "omp.h" 
namespace loopsuite
{

class StructuredDomain 
{
public: int m_npnl;
int m_kmax;
int m_jmax;
}
;

class DelVolBaseLoopAlgorithm 
{
public: void loopExecute();
private: int m_jp;
int m_kp;
protected: const class StructuredDomain *m_Domain;
}
;

void DelVolBaseLoopAlgorithm::loopExecute()
{
int j;
int k;
#pragma omp parallel for private (j,k)
for (k = (this) -> m_Domain -> m_npnl; k <= (this) -> m_Domain -> m_kmax - 1; k += 1) {
  
#pragma omp parallel for private (j)
  for (j = (this) -> m_Domain -> m_npnl; j <= (this) -> m_Domain -> m_jmax - 1; j += 1) {
    int off = j * (this) -> m_jp + k * (this) -> m_kp;
  }
}
}
}
