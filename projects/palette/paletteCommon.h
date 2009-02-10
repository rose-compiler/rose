#ifndef PALETTE_COMMON_H
#define PALETTE_COMMON_H

//#include "rose.h"
#include "Cxx_GrammarMemoryPoolSupport.h"
#include "compass.h"
#include <iostream>
#include <sstream>
#include <string>
#include <sys/resource.h>
#include <sys/time.h>
#include <cmath>
#include <set>
#include <map>
#include <vector>
#include <utility>

  // This is based on the ROSE memory pool traversal code, but changed to run the body inline
#define PALETTE_ITERATE_THROUGH_MEMORY_POOL(type, elt_name) \
        if (type##_Memory_Block_List.empty() == false) \
          for (unsigned int i=0; i < type##_Memory_Block_List.size(); i++) \
            for (int j=0; j < type##_CLASS_ALLOCATION_POOL_SIZE; j++) \
              if (((type**) (&(type##_Memory_Block_List[0])))[i][j].get_freepointer() == AST_FileIO::IS_VALID_POINTER()) \
                if (type* elt_name = &((type**) &(type##_Memory_Block_List[0]))[i][j])

namespace Palette {

  struct Timer {
    double lastTimeCPU, lastTimeWall;
    std::string label;

    Timer(const std::string& label): label(label) {
      rusage ru;
      timeval tv;
      getrusage(RUSAGE_SELF, &ru);
      gettimeofday(&tv, NULL);
      lastTimeCPU = ru.ru_utime.tv_sec + ru.ru_utime.tv_usec * 1.e-6;
      lastTimeWall = tv.tv_sec + tv.tv_usec * 1.e-6;
    }

    ~Timer() {
      rusage ru;
      timeval tv;
      getrusage(RUSAGE_SELF, &ru);
      gettimeofday(&tv, NULL);
      double tCPU = ru.ru_utime.tv_sec + ru.ru_utime.tv_usec * 1.e-6;
      double tWall = tv.tv_sec + tv.tv_usec * 1.e-6;
      double intervalCPU = tCPU - lastTimeCPU;
      double intervalWall = tWall - lastTimeWall;
      std::cerr << label << " took " << intervalCPU << "s CPU time, " << intervalWall << "s wall time" << std::endl;
    }
  };

  class CheckerOutput: public Compass::OutputViolationBase {
    public:
      CheckerOutput(SgNode* node, const std::string& checkerName,
		    const std::string& str):
	Compass::OutputViolationBase(node, checkerName, str) {}
  };

  struct PaletteGreater { // With a templated operator() and only requiring < from its operand
    template <typename T>
    bool operator()(const T& x, const T& y) const {
      return y < x;
    }
  };

}

#endif // PALETTE_COMMON_H
