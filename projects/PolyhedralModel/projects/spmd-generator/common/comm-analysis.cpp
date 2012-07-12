
#include "common/comm-analysis.hpp"
#include "common/array-analysis.hpp"

SyncDescriptor::SyncDescriptor() :
  waiters(),
  waitees()
{}

SyncDescriptor::~SyncDescriptor() {}

void SyncDescriptor::addWaiter(ComputeSystem * cs) {
  waiters.push_back(cs);
}

void SyncDescriptor::addWaitee(ComputeSystem * cs) {
  waitees.push_back(cs);
}

CommDescriptor::CommDescriptor(
  ComputeSystem * source_,
  ComputeSystem * destination_,
  ArrayPartition * data_,
  SyncRequired sync_
) :
  source(source_),
  destination(destination_),
  data(data_),
  sync(sync_)
{}

CommDescriptor::~CommDescriptor() {
  delete data;
}

ComputeSystem * CommDescriptor::getSource() const { return source; }

ComputeSystem * CommDescriptor::getDestination() const { return destination; }

ArrayPartition * CommDescriptor::getArrayPartition() const { return data; }

SyncRequired CommDescriptor::getSyncRequired() const { return sync; }

