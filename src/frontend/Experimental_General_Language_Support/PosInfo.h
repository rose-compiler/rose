#ifndef POS_INFO_H_
#define POS_INFO_H_

class SgLocatedNode;

namespace Rose {
namespace builder {

class PosInfo {
public:
  PosInfo() : startLine_(0), startCol_(0), endLine_(0), endCol_(0) {
  }

  PosInfo(int strtLine, int strtCol, int endLine, int endCol)
    : startLine_{strtLine}, startCol_{strtCol}, endLine_{endLine}, endCol_{endCol} {
  }

  PosInfo(SgLocatedNode* fromNode);

  int  getStartLine() const     { return startLine_; }
  int  getStartCol()  const     { return startCol_;  }
  int  getEndLine()   const     { return endLine_;   }
  int  getEndCol()    const     { return endCol_;    }

  void setStartLine (int line)  { startLine_ = line; }
  void setStartCol  (int col )  { startCol_  = col;  }
  void setEndLine   (int line)  { endLine_   = line; }
  void setEndCol    (int col )  { endCol_    = col;  }

protected:
  int startLine_, startCol_;  // location (line,col) of first character ( 1 based)
  int endLine_,   endCol_;    // location (line,col) of last  character (+1 col)
};

} // namespace builder
} // namespace Rose

#endif // POS_INFO_H_
