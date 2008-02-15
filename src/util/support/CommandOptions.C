#include <general.h>
#include <CommandOptions.h>
#include <iostream>

using namespace std;

CmdOptions* CmdOptions::inst = 0;
CmdOptions* CmdOptions::GetInstance()
{
  if (inst == 0) 
     inst = new CmdOptions();
  return inst;
} 

size_t CmdOptions:: HasOption( const string& opt) const
{
// DQ (1/16/2008): Brian White ask that this output be removed!
// cerr << "Searching for option " << opt << endl;
  assert (cmd);
  vector<string>::const_iterator i =
    std::find(cmd->begin(), cmd->end(), opt);
  if (i == cmd->end()) {
// DQ (1/16/2008): Brian White ask that this output be removed!
 // cerr << "Not found" << endl;
    return 0;
  } else {
// DQ (1/16/2008): Brian White ask that this output be removed!
//  cerr << "Found at position " << i - cmd->begin() << endl;
    return i - cmd->begin() + 1;
   }
}

