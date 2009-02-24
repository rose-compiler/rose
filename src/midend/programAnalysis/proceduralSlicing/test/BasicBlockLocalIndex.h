#ifndef BASICBLOCKATTRIBUTE_IS_DEFINED
#define BASICBLOCKATTRIBUTE_IS_DEFINED



/*!
Local index counter within a basic block.
*/

class BasicBlockLocalIndex : public AstAttribute{
  
 public:
  BasicBlockLocalIndex(int i):local_index(i){};
  void setAttribute(int i){local_index = i;}  
  
 private:
  int local_index;
  
};
// The local index serves as a kind of counter which counts the number of statements within a basic block which already have been "wrapped" in pragma declarations.

#endif
