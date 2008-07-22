#include <general.h>
#include <VectorCommandOptions.h>
#include <iostream>
#include <algorithm>

using namespace std;

VectorCmdOptions* VectorCmdOptions::inst = 0;
VectorCmdOptions* VectorCmdOptions::GetInstance()
{
  if (inst == 0) 
     inst = new VectorCmdOptions();
  return inst;
} 

size_t
VectorCmdOptions:: HasOption( const string& opt) const
{
// DQ (1/16/2008): Brian White ask that this output be removed!
// cerr << "Searching for option " << opt << endl;
  assert (cmd);
  vector<string>::const_iterator i = find(cmd->begin(), cmd->end(), opt);
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

