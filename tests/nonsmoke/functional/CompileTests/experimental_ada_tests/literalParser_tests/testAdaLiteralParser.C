#include "rose.h"
#include "sageInterfaceAda.h"

#include <sstream>
#include <boost/algorithm/string/trim.hpp>

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
    if (!eof(ss))
    {
      std::string rem;
      ss >> rem;
      std::cerr << res << " .. " << rem << std::endl;
    }

    return res;
  }

  template <>
  auto read<std::string>(std::istream& stream, char sep) -> std::string
  {
    return readStr(stream, sep);
  }
  

  template <class T>
  T convertLiteral(const char* val) { ROSE_ASSERT(false); return T(); }
  
  template <>
  int 
  convertLiteral<int>(const char* val) { return si::ada::convertIntLiteral(val); }
  
  template <>
  long double 
  convertLiteral<long double>(const char* val) { return si::ada::convertRealLiteral(val); }
  
  template <>
  std::string 
  convertLiteral<std::string>(const char* val) { return si::ada::convertStringLiteral(val); }
  
  template <class T>
  bool eq(const T& lhs, const T& rhs) 
  { 
    return lhs == rhs; 
  }
  
  template <>
  bool 
  eq<long double>(const long double& lhs, const long double& rhs) 
  { 
    constexpr long double tolerance = .0001;
  
    return std::abs(lhs - rhs) < tolerance; 
  }
  
  template <>
  bool 
  eq<std::string>(const std::string& lhs, const std::string& rhs) 
  { 
    return boost::trim_left_copy(lhs) == boost::trim_left_copy(rhs);
  }

  template <class T>
  void checkConversion(std::istream& is)
  {
    const T           valAsNum     = read<T>(is);
    const std::string valAsStr     = boost::trim_left_copy(read<std::string>(is));
    const T           valAdaParser = convertLiteral<T>(valAsStr.c_str());

    if (!eq(valAsNum, valAdaParser))
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
    else if (kind == "string")
      checkConversion<std::string>(is); 
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
    
    // std::cerr << line << " passed." << std::endl;
  } 

  return 0;
}
