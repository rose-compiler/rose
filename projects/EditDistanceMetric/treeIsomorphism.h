

bool labelsMatch( std::vector< std::vector< SgNode* > > childLevelTraversalTraceList );

void diff ( SgNode* subtree1, SgNode* subtree2 );

void outputAST( SgProject* project );


void traceClone(const float* swTable, char* swDirs, const vector<int> & in1data, const vector<int> & in2data, size_t x, size_t y);
void sequenceAlignment ( vector< vector< SgNode* > > traversalTraceList );

void heatmap(float f, uint8_t& r, uint8_t& g, uint8_t& b);

