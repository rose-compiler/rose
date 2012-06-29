
#ifndef __COMM_ANALYSIS_HPP__
#define __COMM_ANALYSIS_HPP__

#include <vector>

class ComputeSystem;
class ArrayPartition;

enum CommDirection {
  comm_host_to_device,   // from one host to one of its devices
  comm_device_to_host,   // to one host from one of its devices
  comm_host_to_host,     // from one host to another (including himself)
  comm_device_to_device  // from one device to another (can be accross host) (basically encompass a chain of the three other)
};

enum SyncType {
  sync_device_local,   // sync the "local" group of a device (native device's sync for CUDA/OpenCL)
  sync_device_globlal, // sync the whole device (no native support in CUDA/OpenCL)
  sync_host_to_device, // sync one host on one of its device
  sync_host_to_host    // sync between a group of host 
};

class SyncDescriptor {
  protected:
    std::vector<ComputeSystem *> waiters;
    std::vector<ComputeSystem *> waitees;

  public:
    SyncDescriptor();
    virtual ~SyncDescriptor();

    void addWaiter(ComputeSystem * cs);
    void addWaitee(ComputeSystem * cs);
};

enum SyncRequired {
  none,
  before,
  after
};

class CommDescriptor {
  protected:
    ComputeSystem * source;
    ComputeSystem * destination;

    ArrayPartition * data;

    SyncRequired sync;

  public:
    CommDescriptor(
      ComputeSystem * source_,
      ComputeSystem * destination_,
      ArrayPartition * data_,
      SyncRequired sync_ = none
    );
    virtual ~CommDescriptor();

    ComputeSystem * getSource() const;
    ComputeSystem * getDestination() const;

    ArrayPartition * getArrayPartition() const;

    SyncRequired getSyncRequired() const;
};

#endif /* __COMM_ANALYSIS_HPP__ */
