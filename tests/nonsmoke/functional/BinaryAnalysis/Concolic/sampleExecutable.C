
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


typedef std::pair<std::string, std::string> KeyValue;

static const std::string check_env = "--env=";
static const std::string check_rndaddr = "--address-randomization=";

void fail() { std::exit(1); }

KeyValue keyValue(const std::string& arg)
{
  // starting positions in arg for key and value
  const size_t val = arg.find('=');

  if (val == std::string::npos)
    fail();

  return std::make_pair(arg.substr(0, val), arg.substr(val+1));
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
  return false;
#endif /* __linux__ */
}

void check(std::string arg)
{
  if (arg.find(check_env) == 0)
  {
    KeyValue    dsc = keyValue(arg.substr(check_env.size()));
    const char* val = std::getenv(dsc.first.c_str());

    if (!val || dsc.second != val)
      fail();

    return;
  }

  if (arg.find(check_rndaddr) == 0)
  {
    const bool  expected = trueFalseValue(arg.substr(check_rndaddr.size()));
    const bool  rndaddr  = addressRandomization();

    if (!onLinux || (expected != rndaddr))
      fail();

    return;
  }
}

int main(int argc, char** argv)
{
  for (int i = 1; i < argc; ++i)
    check(argv[i]);

  return 0;
}
