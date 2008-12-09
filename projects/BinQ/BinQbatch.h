#ifndef batch_GUI_H
#define batch_GUI_H

#include "BinQAbstract.h"

class BinQbatch : public BinQAbstract
{
  public:
  BinQbatch(std::string, std::string,     
	  std::vector<std::string> dllA,     
	  std::vector<std::string> dllB);
    ~BinQbatch();

    void testAnalyses();


}; //class BinQGUI

#endif
