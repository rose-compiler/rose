#ifndef RSIM_CloneDetectionOutputs_H
#define RSIM_CloneDetectionOutputs_H

#include "CloneDetectionAnalysisFault.h"
#include "threadSupport.h"
#include "sqlite3x.h"


#include <stdint.h>
#include <vector>
#include <ostream>
#include <map>

namespace CloneDetection {

typedef std::map<int, uint64_t> IdVa;
typedef std::map<uint64_t, int> VaId;

/** Collection of output values. The output values are gathered from the instruction semantics state after a specimen function
 *  is analyzed.  The outputs consist of those interesting registers that are marked as having been written to by the specimen
 *  function, and the memory values whose memory cells are marked as having been written to.  We omit status flags since they
 *  are not typically treated as function call results, and we omit the instruction pointer (EIP). */
class OutputGroup {
public:
    typedef uint32_t value_type;
    OutputGroup(): fault(AnalysisFault::NONE) {}
    std::vector<value_type> values;
    std::vector<value_type> callees_va;         // virtual address of every function call
    std::vector<int> syscalls;                  // system call numbers in the order they occur
    AnalysisFault::Fault fault;
    bool operator==(const OutputGroup &other) const;
    void print(std::ostream&, const std::string &title="", const std::string &prefix="") const;
    void print(RTS_Message*, const std::string &title="", const std::string &prefix="") const;
    friend std::ostream& operator<<(std::ostream &o, const OutputGroup &outputs) {
        outputs.print(o);
        return o;
    }
};

typedef std::vector<OutputGroup> OutputGroups;

void load_output_groups(sqlite3x::sqlite3_connection&, const IdVa*, OutputGroups&/*out*/);

} // namespace
#endif
