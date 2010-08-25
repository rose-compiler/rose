
#ifndef BDWY_LIVENESS_H
#define BDWY_LIVENESS_H

#include "liveness.h"

/** @brief Liveness analyzer
 *
 * This class is simply an extension of the standard liveness analysis that
 * uses the annotations to determine if entire library calls are dead. */

class bdwyLivenessAnalyzer : public livenessAnalyzer
{
private:

  stmtLocation * _library_stmt;

  bool _automatically_live;

public:

  bdwyLivenessAnalyzer();

  void clear();

  void record_defs(procLocation * libproc_location,
                   procedureAnn * libproc_anns);

  virtual void at_threeAddr(stmtLocation * stmt,
			    threeAddrNode * threeaddr,
			    pointerValue & result);
};

#endif /* BDWY_LIVENESS_H */
