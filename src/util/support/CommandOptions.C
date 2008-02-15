#include <CommandOptions.h>

CmdOptions* CmdOptions::inst = 0;
CmdOptions* CmdOptions::GetInstance()
{
  if (inst == 0) 
     inst = new CmdOptions();
  return inst;
} 

void CmdOptions::SetOptions  (int argc, char* argv[])
   {
     for (int i = 1; i < argc; ++i) {
       cmd = cmd + " " + argv[i];
       if ( argv[i] == 0)
           continue;
     }
     cmd = cmd + " ";
  }

const char* CmdOptions:: HasOption( const STD string& opt)
{
  const char* p = STD strstr(cmd.c_str(), opt.c_str()); 
  if (p != 0 && *(p+opt.size()) == ' ') {
      return p;
   }
  return 0;
}

