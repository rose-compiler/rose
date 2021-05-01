#ifndef __ROSE_MEMPOOL_SNAPSHOT_H__
#define __ROSE_MEMPOOL_SNAPSHOT_H__

#include <ostream>
#include <string>

namespace Rose {
namespace MemPool {

void snapshot(std::ostream &);
void snapshot(std::string const &);

}
}

#endif /* __ROSE_MEMPOOL_SNAPSHOT_H__ */

