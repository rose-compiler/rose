#include <CommandOptions.h>
#include <cassert>

CmdOptions* CmdOptions::inst = 0;
CmdOptions* CmdOptions::GetInstance()
{
  if (inst == 0) 
     inst = new CmdOptions();
  return inst;
} 

void CmdOptions::SetOptions  (const std::vector<std::string>& opts)
   {
     assert (!opts.empty());
     this->opts = opts;
     this->opts.erase(this->opts.begin());
  }

void CmdOptions::SetOptions  (int argc, const char* argv[]) {
  this->SetOptions(std::vector<std::string>(argv, argv + argc));
}

void CmdOptions::SetOptions  (int argc, char* argv[]) {
  this->SetOptions(std::vector<std::string>(argv, argv + argc));
}

bool CmdOptions:: HasOption( const std::string& opt)
{
  for (std::vector<std::string>::const_iterator i = opts.begin();
       i != opts.end(); ++i) {
    if ((*i).substr(0, opt.size()) == opt) return true;
  }
  return false;
}

std::vector<std::string>::const_iterator CmdOptions:: GetOptionPosition( const std::string& opt)
{
  for (std::vector<std::string>::const_iterator i = opts.begin();
       i != opts.end(); ++i) {
    if ((*i).substr(0, opt.size()) == opt) return i;
  }
  return opts.end();
}

