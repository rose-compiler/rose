
#include "compute-systems/cluster-system.hpp"

LAN::LAN() :
  Link()
{}

LAN::LAN(const LAN & arg) :
  Link(arg)
{}

LAN::~LAN() {}

Cluster::Cluster(std::vector<unsigned> & dimensions_, ComputeSystem * element, LAN * interconnection) :
  ArraySystem(dimensions_, element, NULL, interconnection, NULL)
{}

Cluster::Cluster(const Cluster & arg) :
  ArraySystem(arg)
{}

Cluster::~Cluster() {}

Cluster * Cluster::copy() const {
  return new Cluster(*this);
}

