#ifndef ANALYSISASTATTRIBUTE
#define ANALYSISASTATTRIBUTE

#include <string>
using std::string;

class AnalysisResultAttribute : public AstAttribute {
 public:
  string virtual getPreInfoString();
  string virtual getPostInfoString();
  ~AnalysisResultAttribute();
};

#endif
