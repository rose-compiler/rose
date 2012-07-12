#include <CommandOptions.h>
#include <cassert>

CmdOptions* CmdOptions::inst = 0;
CmdOptions* CmdOptions::GetInstance()
{
  if (inst == 0) 
     inst = new CmdOptions();
  return inst;
} 

void CmdOptions::SetOptions  (const std::vector<std::string>& _opts)
   { opts=_opts; }

void CmdOptions::SetOptions  (int argc, const char* argv[]) {
  SetOptions(std::vector<std::string>(argv, argv + argc));
}

void CmdOptions::SetOptions  (int argc, char* argv[]) {
  SetOptions(std::vector<std::string>(argv, argv + argc));
}

bool CmdOptions:: HasOption( const std::string& opt)
{
  for (std::vector<std::string>::const_iterator i = opts.begin();
       i != opts.end(); ++i) {
    if ((*i) == opt) return true;
  }
  return false;
}

std::vector<std::string>::const_iterator CmdOptions:: GetOptionPosition( const std::string& opt)
{
  for (std::vector<std::string>::const_iterator i = opts.begin();
       i != opts.end(); ++i) {
    if ((*i) == opt) return i;
  }
  return opts.end();
}

