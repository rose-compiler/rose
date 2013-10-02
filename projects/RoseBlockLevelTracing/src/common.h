#include <stdint.h>


// TraceId structure.
// File id is a machine-wide unique id derived from a rose database.
// Node id is a id given to each node during the instrumentation phase.
struct TraceId {
    uint32_t fileId;
    uint32_t nodeId;
    TraceId(uint32_t fid, uint32_t nid): fileId(fid), nodeId(nid){}
    TraceId(uint64_t uniqueId): fileId(uniqueId>>32), nodeId(uniqueId & 0xffffffff){}
    uint64_t GetSerializableId() {
        return (uint64_t(fileId) << 32) | nodeId;
    }
};

