#include "rose.h"
#include "sageInterfaceAda.h"

#include <sstream>

namespace si = SageInterface;

namespace
{
  bool eof(std::istream& stream)
  {
    std::string tmp;
    stream >> tmp;
  
    return tmp == "" && stream.eof();
  }


  auto readStr(std::istream& stream, char sep = ',') -> std::string
  {
    std::string el;

    getline(stream, el, sep);
    return el;
  }

  template <class T>
  auto read(std::istream& stream, char sep = ',') -> T
  {
    std::stringstream ss(readStr(stream, sep));
    T                 res;
    
    ss >> res;
    assert(eof(ss));
    return res;
  }

  template <class T>
  T convertNumber(const char* val) { ROSE_ASSERT(false); return T(); }
  
  template <>
  int 
  convertNumber<int>(const char* val) { return si::ada::convertIntLiteral(val); }
  
  template <>
  long double 
  convertNumber<long double>(const char* val) { return si::ada::convertRealLiteral(val); }
  
  template <class T>
  T tolerance() { ROSE_ASSERT(false); return T(); }
  
  template <>
  int 
  tolerance<int>() { return 0; }
  
  template <>
  long double 
  tolerance<long double>() { return .0001; }

  template <class T>
  void checkConversion(std::istream& is)
  {
    const T           valAsNum     = read<T>(is);

    const std::string valAsStr     = read<std::string>(is);
    const T           valAdaParser = convertNumber<T>(valAsStr.c_str());

    if (std::abs(valAsNum - valAdaParser) > tolerance<T>())
    {
      std::cerr << std::endl << valAsNum << " " << valAdaParser << "(" << valAsStr << ")" << std::endl;

      throw std::runtime_error("value mismatch between computed and expected number"); 
    }
  }

  void checkLine(std::istream& is)
  {
    const std::string kind = read<std::string>(is);

    if (kind == "int")
      checkConversion<int>(is);
    else if (kind == "real")
      checkConversion<long double>(is);  
  }
}


int main( int argc, char * argv[] )
{
  // Initialize and check compatibility. See Rose::initialize
  ROSE_INITIALIZE;

  if (argc < 2) return 0;

  std::ifstream inputfile{argv[1]};

  for (;;) 
  {
    std::string line = readStr(inputfile, '\n');
    
    // break on eof
    if (!inputfile) break;

    if (line.size() == 0 || line[0] == '#')
      continue;
    
    std::stringstream linestream(line);

    checkLine(linestream);
    assert(eof(linestream));
  } 

  return 0;
}
