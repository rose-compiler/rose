#ifndef batch_GUI_H
#define batch_GUI_H

#include "BinQGui.h"

class BinQbatch : public BinQGUI
{
  public:
  BinQbatch(std::string, std::string,     
	  std::vector<std::string> dllA,     
	  std::vector<std::string> dllB, bool test);
    ~BinQbatch();

    void runAnalyses(std::vector<BinAnalyses*>& analysesVec, bool init);
    void initAnalyses();
    int addRemainingAnalyses();
}; //class BinQGUI

#endif
