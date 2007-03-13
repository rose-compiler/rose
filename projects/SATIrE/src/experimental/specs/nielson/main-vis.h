#ifndef MAIN_VIS_INCLUDED
#define MAIN_VIS_INCLUDED
/* Specify the carrier type of the analysis. This should be the same
 * as the one given with "carrier:" in the optla file.
 * For example: #define CARRIER_TYPE State */

#ifndef CARRIER_TYPE
    #define CARRIER_TYPE PLEASE_SPECIFY_CARRIER_TYPE
#endif

#include "main.h"
#include "cfg_support.h"
#include "analysis_info.h"

class AliasTraversal : public AstSimpleProcessing
{
public:
    AliasTraversal(DFI_STORE store_) : store(store_) {}

protected:
    void visit(SgNode *);

private:
    DFI_STORE store;
};

#endif
