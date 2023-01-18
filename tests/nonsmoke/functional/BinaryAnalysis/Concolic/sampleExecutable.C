#include <csignal>
#include <cstdlib>
#include <sstream>
#include <iostream>

#if defined(__linux__)
#include <sys/personality.h>

static const unsigned long QUERY_PERSONA = 0xffffffff;

static const bool onLinux = true;
#else
static const bool onLinux = false;
#endif /* __linux__ */

enum ExitStatus {
    FAIL_MALFORMED_ENV = 1,
    FAIL_ENV_UNDEFINED = 2,
    FAIL_ENV_NONMATCH = 3,
    FAIL_NOT_LINUX = 4,
    FAIL_BAD_RANDOMIZATION = 5,
};

typedef std::pair<std::string, std::string> KeyValue;

static const std::string check_env      = "--env=";
static const std::string check_rndaddr  = "--address-randomization=";
static const std::string check_segfault = "--seg-fault";
static const std::string check_exitcode = "--exit=";

void fail(const std::string &mesg, const ExitStatus exitStatus) {
    if (!mesg.empty())
        std::cerr <<"specimen error: " <<mesg <<"\n";
    std::exit((int)exitStatus);
}

KeyValue keyValue(const std::string& arg)
{
  // starting positions in arg for key and value
  const size_t val = arg.find('=');

  if (val == std::string::npos)
      fail("cannot find \"=\" in \"" + arg + "\"\n", FAIL_MALFORMED_ENV);
    
  return std::make_pair(arg.substr(0, val), arg.substr(val+1));
}

int intValue(const std::string& arg)
{
  std::stringstream buf;
  int               res;
  
  buf << arg;
  buf >> res;
  
  return res;    
}

bool trueFalseValue(const std::string& arg)
{
  return (arg == "true" || arg == "on");
}

bool addressRandomization()
{
#if defined(__linux__)
  return (personality(QUERY_PERSONA) & ADDR_NO_RANDOMIZE) == 0;
#else
  fail("personality only exists on Linux", FAIL_NOT_LINUX);
#endif /* __linux__ */
}

void segFault() {
#if 0 // [Robb Matzke 2020-01-15]: this is C++ undefined behavior and might not do what you expect.
  void (*fun)() = NULL;
  fun();
#else
  raise(SIGSEGV);
#endif
}

void check(std::string arg)
{
  if (arg.find(check_env) == 0)
  {
    KeyValue    dsc = keyValue(arg.substr(check_env.size()));
    const char* val = std::getenv(dsc.first.c_str());

    if (!val)
        fail("environment variable \"" + dsc.first + "\" is not defined", FAIL_ENV_UNDEFINED);

    if (dsc.second != std::string(val)) {
        fail("environment variable \"" + dsc.first + "\" value \"" + std::string(val) + "\""
             " does not match \"" + dsc.second + "\"",
             FAIL_ENV_NONMATCH);
    }

    return;
  }

  if (arg.find(check_rndaddr) == 0)
  {
    const bool  expected = trueFalseValue(arg.substr(check_rndaddr.size()));
    const bool  rndaddr  = addressRandomization();

    if (!onLinux)
        fail("not running on Linux", FAIL_NOT_LINUX);

    if (expected != rndaddr) {
        fail("address randomization was " + std::string(rndaddr ? "true" : "false") +
             " but was expected to be " + std::string(expected ? "true" : "false"),
             FAIL_BAD_RANDOMIZATION);
    }

    return;
  }
  
  if (arg.find(check_exitcode) == 0)
  {
    const int exitcode = intValue(arg.substr(check_exitcode.size()));
    
    exit(exitcode);
  }
  
  if (arg.find(check_segfault) == 0)
  {
    segFault();
  }
}

int main(int argc, char** argv)
{
  for (int i = 1; i < argc; ++i)
    check(argv[i]);

  return 0;
}
