
#ifndef __CLUSTER_SYSTEM_HPP__
#define __CLUSTER_SYSTEM_HPP__

#include "common/compute-system.hpp"
#include "compute-systems/compute-node.hpp"

class LAN : public Link {
  public:
    LAN();
    LAN(const LAN & arg);
    virtual ~LAN();

    virtual LAN * copy() const = 0;
};

class Cluster : public ArraySystem {
  public:
    Cluster(std::vector<unsigned> & dimensions_, ComputeSystem * element, LAN * interconnection);
    Cluster(const Cluster & arg);
    virtual ~Cluster();

    virtual Cluster * copy() const;
};

#endif /* __CLUSTER_SYSTEM_HPP__ */

