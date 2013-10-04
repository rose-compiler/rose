
// Declare this global variable so that other analysis can reference it.
// Later this might be a reference to SgFile or SgGlobal in SgFile.
extern GlobalScopeAnalysisAttribute* globalScopeAttribute;

void addGlobalScopeAnalysisAttributes( SgProject* project );

void addScopeAnalysisAttributes( SgProject* project );


