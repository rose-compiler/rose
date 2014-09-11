#ifndef	OFP_UNPARSER_HPP
#define	OFP_UNPARSER_HPP

#include "OFPNodes.hpp"
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>

namespace OFP {

class Unparser;

void setUnparser(Unparser* u);

class UnparseInfo
{
 public:
   UnparseInfo() : level(0)  {}
   virtual ~UnparseInfo()    {}

   int indentLevel() {return level;}

 private:
   int level;
};

class Unparser
{
 public:
   Unparser() : oss(std::cout) {}
   Unparser(std::ostream & os) : oss(os) {}
   Unparser(UnparseInfo & infoObj) : oss(std::cout) {info = infoObj;}
   virtual ~Unparser() {}

   virtual void unparseNode(SgUntypedNode * node) = 0;

 protected:
   std::ostream & oss;

 private:
   UnparseInfo info;
};

class FortranTextUnparser : public Unparser
{
 public:
   FortranTextUnparser()                                  {}
   FortranTextUnparser(std::ostream & os) : Unparser(os)  {}
   FortranTextUnparser(UnparseInfo & infoObj)             {}
  ~FortranTextUnparser()                                  {}

   void unparseNode(SgUntypedNode * node);

 private:
   
};

} // namespace OFP

#endif
