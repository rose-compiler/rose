#ifndef interactive_GUI_H
#define interactive_GUI_H

#include "BinQAbstract.h"
#include "BinQGui.h"

class BinQinteractive : public BinQGUI
{
  public:
  BinQinteractive(std::string, std::string,     
	  std::vector<std::string> dllA,     
	  std::vector<std::string> dllB, bool test);
    ~BinQinteractive();

    //void testAnalyses();
    void initAnalyses();

}; //class BinQGUI

#endif
