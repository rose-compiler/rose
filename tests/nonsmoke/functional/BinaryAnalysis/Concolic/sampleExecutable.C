
#include <cstdlib>
#include <sstream>
#include <iostream>

typedef std::pair<std::string, std::string> KeyValue;

static const std::string check_env = "--env=";

//~ template <class T, class S>
//~ T as(const S& src)
//~ {
  //~ std::stringstream str;
  //~ T                 tgt;

  //~ str << src;
  //~ str >> tgt;
  //~ return tgt;
//~ }

void fail() { std::exit(1); }

KeyValue keyValue(const std::string& arg, const std::string& cmdldesc)
{
  // starting positions in arg for key and value
  const size_t key = cmdldesc.size();
  const size_t val = arg.find('=', key);

  if (val == std::string::npos)
    fail();

  return std::make_pair(arg.substr(key, val-key), arg.substr(val+1));
}

void check(std::string arg)
{
  if (arg.find(check_env) == 0)
  {
    KeyValue    dsc = keyValue(arg, check_env);
    const char* val = std::getenv(dsc.first.c_str());

    if (!val || dsc.second != val)
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
