#ifndef _PERF_OUTPUT_ANALYZER_HPP
#define _PERF_OUTPUT_ANALYZER_HPP 1

struct location_t
{
  const char* binary;
  const char* address; 
};

struct ctrl_transfer_t
{
  location_t origin;
  location_t target;
};

struct AddrIntervalSet;

struct execution_range_t
{
  location_t       last;
  AddrIntervalSet* coderanges;
};

location_t        errorLocation();
ctrl_transfer_t   errorTransfer();
execution_range_t range(execution_range_t prior, ctrl_transfer_t transf);
execution_range_t range();
void report(execution_range_t);

#endif /* _PERF_OUTPUT_ANALYZER_HPP */
