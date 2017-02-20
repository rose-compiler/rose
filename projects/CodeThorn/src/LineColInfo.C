#include "sage3basic.h"
#include "LineColInfo.h"

LineColInfo::LineColInfo(SgLocatedNode* locNode) {
  int edgCorr=-1;
  startLine=locNode->get_startOfConstruct()->get_raw_line();//+edgCorr;
  startCol=locNode->get_startOfConstruct()->get_raw_col()+edgCorr;
  endLine=locNode->get_endOfConstruct()->get_raw_line();//+edgCorr;
  endCol=locNode->get_endOfConstruct()->get_raw_col()+edgCorr;
}

std::string LineColInfo::toString() { 
  using namespace std;
  std::stringstream ss;
#if 0
  ss<<setw(3)<<setfill('0')<<startLine<<","
    <<setw(3)<<setfill('0')<<startCol<<"=>"
    <<setw(3)<<setfill('0')<<endLine<<","
    <<setw(3)<<setfill('0')<<endCol;
#else
  ss<<startLine<<","
    <<startCol<<" => "
    <<endLine<<","
    <<endCol;
#endif
  return ss.str();
}
