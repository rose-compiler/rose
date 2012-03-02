#include <config.h>
#include <rose.h>

#include <map>
#include "cfg_support.h"

class CFGCheck {
public:
  CFGCheck();
  void checkExpressions(CFG* cfg);
  void displayResultsOn();
  void displayResultsOff();
  bool displayResults();
  void print(std::string s);
  void printnl(std::string s);
  void printnl();
private:
  bool _displayResults;
};
