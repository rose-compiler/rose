#ifndef LINE_COL_INFO_H
#define LINE_COL_INFO_H

class SgLocatedNode;

#include <string>

struct LineColInfo {
  LineColInfo(SgLocatedNode* locNode);
  std::string toString();
private:
  int startLine;
  int startCol;
  int endLine;
  int endCol;
};

#endif
