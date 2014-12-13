#ifndef bROwSE_FunctionUtil_H
#define bROwSE_FunctionUtil_H

#include <bROwSE/bROwSE.h>

namespace bROwSE {

enum MayReturn { MAYRETURN_YES, MAYRETURN_NO, MAYRETURN_UNKNOWN };

struct Box { int x, y, dx, dy; };

typedef Sawyer::Container::Map<rose_addr_t, Box> CfgVertexCoords;

size_t functionNBytes(const P2::Partitioner&, const P2::Function::Ptr&);
size_t functionNInsns(const P2::Partitioner&, const P2::Function::Ptr&);
boost::filesystem::path functionCfgGraphvizFile(const P2::Partitioner&, const P2::Function::Ptr&);
boost::filesystem::path functionCfgImage(const P2::Partitioner&, const P2::Function::Ptr&);
CfgVertexCoords functionCfgVertexCoords(const P2::Partitioner&, const P2::Function::Ptr&);
P2::FunctionCallGraph* functionCallGraph(P2::Partitioner&);
size_t functionNCallers(P2::Partitioner&, const P2::Function::Ptr&);
size_t functionNReturns(P2::Partitioner&, const P2::Function::Ptr&);
MayReturn functionMayReturn(P2::Partitioner&, const P2::Function::Ptr&);
int64_t functionStackDelta(P2::Partitioner&, const P2::Function::Ptr&);

} // namespace
#endif
