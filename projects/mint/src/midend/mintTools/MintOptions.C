
#include "MintOptions.h"

#include <CommandOptions.h>
#include <cassert>
#include <iostream>
#include <cstdio>
#include <cassert>
#include <cstdlib>
#include <cstring>

using namespace std;

MintOptions* MintOptions::inst = 0;
MintOptions* MintOptions::GetInstance()
{
  if (inst == 0) {
    inst = new MintOptions();
  }
  return inst;
}

static void allocAndCopyStr(char** dest, const char* src)
{
  *dest = (char*) malloc((strlen(src)+1)*sizeof(char));
  if (strlen(src)!=0)
   assert (*dest != NULL);
  strcpy (*dest, src);
  assert (strlen(src) == strlen (*dest));
}

void  MintOptions::setInternalCommandLineArgs(int &argc, char* argv[])
{
  //for openmp lowering
//  argv[argc]= "-rose:openmp:ast_only";
  allocAndCopyStr (& (argv[argc]), "-rose:openmp:ast_only");
  argc++;

  //we need to linearize arrays on the device
  //argv[argc]= "-linearize";
  allocAndCopyStr (& (argv[argc]), "-linearize");
  
  argc++;

  //argv[argc]= "-opt:useSameIndex";
  allocAndCopyStr (& (argv[argc]), "-opt:useSameIndex");
  argc++;

}

bool MintOptions::optimize()
{
  if(isSharedOpt())
    return true;
  if(isRegisterOpt())
    return true;
  if(isSlidingOpt())
    return true;

  return false;
}

void MintOptions::SetMintOptions  (const std::vector<std::string>& opts)
{
  CmdOptions::GetInstance()->SetOptions(opts);
  assert (!opts.empty());
  this->opts = opts;
  this->opts.erase(this->opts.begin());
}


bool MintOptions:: HasOption( const std::string& opt)
{
  for (std::vector<std::string>::const_iterator i = opts.begin();
       i != opts.end(); ++i) {
    if ((*i).substr(0, opt.size()) == opt) return true;
  }
  return false;
}

bool MintOptions::isSharedOpt()
{
  static int r = 0;
  if (r == 0) {
    if (HasOption("-opt:shared"))
      r = 1;
    else
      r = -1;
  }
  return r == 1;
}

int MintOptions::getNumberOfSharedMemPlanes()
{
  vector<string>::const_iterator config = CmdOptions::GetInstance()->GetOptionPosition("-opt:shared");

  size_t numplanes=0;

  if (config == CmdOptions::GetInstance()->end())
    return 0;

  assert (config != CmdOptions::GetInstance()->end());

  string sharedmem_param = (*config).c_str() ;

  int cutAt = sharedmem_param.find_first_of("=");

  if(cutAt > 0){ //shared memory size is given 
    //get the substring 
    string tmp = sharedmem_param.substr(cutAt+1, sharedmem_param.length());
    sscanf(tmp.c_str(), "%zu", &numplanes);
  }
  
  return (numplanes <= 0 ) ? 6 : numplanes;
}

bool MintOptions::isSwapOpt()
{
  static int r = 0;
  if (r == 0) {
    if (CmdOptions::GetInstance()->HasOption("-opt:swap"))
      r = 1;
    else
      r = -1;
  }
  return r == 1;
}


bool MintOptions::useSameIndex()
{
  static int r = 0;
  if (r == 0) {
    if (CmdOptions::GetInstance()->HasOption("-opt:useSameIndex"))
      r = 1;
    else
      r = -1;
  }
  return r == 1;
}


bool MintOptions::linearize()
{
  static int r = 0;
  if (r == 0) {
    if (CmdOptions::GetInstance()->HasOption("-linearize"))
      r = 1;
    else
      r = -1;
  }
  return r == 1;
}

bool MintOptions::isSlidingOpt()
{
  static int r = 0;
  if (r == 0) {
    if (CmdOptions::GetInstance()->HasOption("-opt:sliding"))
      r = 1;
    else
      r = -1;
  }
  return r == 1;
}

bool MintOptions::isRegisterOpt()
{
  static int r = 0;
  if (r == 0) {
    if (CmdOptions::GetInstance()->HasOption("-opt:register"))
      r = 1;
    else
      r = -1;
  }
  return r == 1;
}

bool MintOptions::isUnrollOpt()
{
  static int r = 0;
  if (r == 0) {
    if (CmdOptions::GetInstance()->HasOption("-opt:unroll"))
      r = 1;
    else
      r = -1;
  }
  return r == 1;
}

bool MintOptions::isL1Preferred()
{
  static int r = 0;
  if (r == 0) {
    if (CmdOptions::GetInstance()->HasOption("-opt:preferL1"))
      r = 1;
    else
      r = -1;
  }
  return r == 1;
}

std::vector<std::string>::const_iterator CmdOptions:: GetOptionPosition( const std::string& opt)
{
  for (std::vector<std::string>::const_iterator i = opts.begin();
       i != opts.end(); ++i) {
    if ((*i).substr(0, opt.size()) == opt) return i;
  }
  return opts.end();
}
