#include <cstdio>
#include <cassert>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <string>

#include "rosedefs.h"
#include "Sawyer/IntervalSet.h"
#include "Sawyer/Interval.h"

#include "analyzer.hpp"
#include "parser.hpp"

//

// flex & bison declarations

struct yy_buffer_state;
extern FILE* yyin;
typedef yy_buffer_state* YY_BUFFER_STATE;

YY_BUFFER_STATE yy_scan_string(const char * str); // it does not work.
YY_BUFFER_STATE yy_scan_buffer(char *, size_t);
void yy_delete_buffer(YY_BUFFER_STATE buffer);
void yy_switch_to_buffer(YY_BUFFER_STATE buffer);

extern char* yytext;
extern int yylex (void);
extern int yyparse(void);

//
// analyzer declarations

typedef rose_addr_t                         addr_t;
typedef Sawyer::Container::Interval<addr_t> AddressInterval;

/** AddressIntervalSet stores executed aadress intervals.
 * 
 * \note AddressIntervalSet is forward declared in analyzer.hpp can therefor not
 *       be a typedef.
 */
struct AddrIntervalSet : Sawyer::Container::IntervalSet<AddressInterval>
{};


namespace /* anonymous */ 
{
  inline
  addr_t hexToNum(const char* s)
  {
    assert(s);
    
    addr_t            res;
    std::stringstream cnv;
    
    cnv << s;
    cnv >> std::hex >> res;
    
    return res;
  }
  
  inline
  std::string str(const char* s)
  {
    return s ? s : "<null>";
  }

  inline  
  void freeLocation(location_t loc)
  {
    free(const_cast<char*>(loc.binary));
    free(const_cast<char*>(loc.address));
  }
}


//
// parser callbacks

location_t errorLocation()
{
  return location_t { NULL, NULL };
}

ctrl_transfer_t errorTransfer()
{
  return ctrl_transfer_t { errorLocation(), errorLocation() };
}

execution_range_t range()
{
  return execution_range_t { errorLocation(), new AddrIntervalSet() };
}

void addCodeRegion(AddrIntervalSet& coderanges, location_t begin, location_t end)
{
  // code region spawns two different binaries
  //   -> ignore
  if (str(begin.binary) != str(end.binary)) return;
    
  // no code address given
  //   -> ignore
  if (!begin.address || !end.address) return;
  
  const addr_t first = hexToNum(begin.address);
  const addr_t last  = hexToNum(end.address);
  
  // one of the code addresses is 0
  //   -> ignore
  if (first == 0 || last == 0) return;
  
  // if the end is before the start, the region is invalid
  //   -> ignore
  if (last < first)
  {
    std::cerr << str(begin.binary) << " @" << str(begin.address)
              << " --> "
              << str(end.binary)   << " @" << str(end.address)
              << std::endl;
    return;
  }
  
  // now do the real work
  coderanges.insert(AddressInterval::hull(first, last));
}

execution_range_t range(execution_range_t prior, ctrl_transfer_t transf)
{
  assert(prior.coderanges);
  
  addCodeRegion(*prior.coderanges, prior.last, transf.origin);
  
  freeLocation(prior.last);
  freeLocation(transf.origin);
    
  return execution_range_t { transf.target, prior.coderanges };
}

void report(execution_range_t range)
{
  std::cout << range.coderanges->size() << std::endl;
}

//
// main calls parser

int main(int argc, char** argv)
{
  yyin = argc == 1 ? stdin : fopen(argv[1], "r"); 

  yyparse();
  
  if (argc > 1) fclose(yyin);
  return 0;
}
