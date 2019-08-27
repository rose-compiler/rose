// NAME
//   execmon - monitors the execution of some program and
//             computes some coverage number.
//
// SYNOPSIS
//   execmon [arguments] -- program [program-arguments]
//
// DESCRIPTION
//   Monitors the execution of a program and produces some coverage number.
//   The coverage number is an estimate how many unique instructions were
//   executed.
//   To this end, execmon traces the execution and records code
//   intervals. Code intervals are non-overlapping, and in some runs
//   contiguously executed code segments.
//
//   Coverage number := sum_{i=0}^{#segments}(instrEstimate(segment_i))
//
//   As opposed to some RISC architectures, other systems (e.g., x86
//   and derivatives) use a variable instruction length. To compute the
//   instrEstimate, execmon assumes an average length (maybe just
//   bytecount would suffice).
//
//   --perf=intel  uses perf on intel systems /* TODO */

#include <algorithm>

#include "sage3basic.h"

//~ #include "Sawyer/Map.h"
//~ #include "Sawyer/AddressMap.h"
#include "Sawyer/IntervalSet.h"

//~ typedef Sawyer::Container::IntervalSet<AddressInterval> AddressIntervalSet

//~ #include "RegisterDescriptor.h"
#include "BinaryDebugger.h"


typedef Sawyer::Container::Interval<unsigned>    AddressInterval;
typedef Sawyer::Container::IntervalSet<Interval> AddressIntervalSet;

struct IsSeparator
{
  bool operator()(const char* str)
  {
    return std::equal(separator, separator + sizeof(separator)/sizeof(char), str);
  }

  static const char separator[3];
};

const char IsSeparator::separator[3] = { '-', '-', 0 };


namespace Rose {
namespace BinaryAnalysis {

  struct ExecMonitor : BinaryDebugger
  {
    typedef size_t addr_t;
    typedef Sawyer::Interval<addr_t> AddressInterval;

    using BinaryDebugger::BinaryDebugger;

    AddressInterval startNewInterval()
    {
      return AddressInterval(pc());
    }

    addr_t pc()
    {
      return 0; // \todo
    }

    static
    bool adjacent(const AddressInterval& curr, addr_t x)
    {
      if (curr.hi > x) return false;

      return (x - curr.hi) <= ARCH_INSTR_SIZE;
    }

    void tiptoe()
    {
      if (adjacent(currIval, pc())
      {
        endpoint(currIval) = pc();
      }
      else
      {
        intervals.insert(currIval);

        currIval = startNewInterval();
      }

      // if not-expected PC
      //   record previous execution range;
      //   + start new execution range

      // record estimate of next PC

      // make a move
      singleStep();
    }

    void run()
    {
      currIval = startNewInterval();

      while (!atEnd())
      {
        tiptoe();
      }
    }

    AddressInterval    currIval;  //< current interval
    AddressIntervalSet intervals;
  };

}
}

int main(int argc, char** argv)
{
  char** sep = std::find_if(argv, argv+argc, IsSeparator());

  return 0;
}




