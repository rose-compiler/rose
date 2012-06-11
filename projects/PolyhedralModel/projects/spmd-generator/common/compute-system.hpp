
#ifndef __COMPUTE_SYSTEM_HPP__
#define __COMPUTE_SYSTEM_HPP__

#include <vector>

class Link {
  protected:
    unsigned id;

  static unsigned id_cnt;

  public:
    Link();
    virtual ~Link();

    unsigned getID() const;

    virtual Link * copy() const = 0;
};

class ComputeSystem {
  protected:
    ComputeSystem * parent;
    unsigned id;

  static unsigned id_cnt;

  public:
    ComputeSystem(ComputeSystem * parent = NULL);
    ComputeSystem(const ComputeSystem & arg);
    virtual ~ComputeSystem();

    unsigned getID() const;
    ComputeSystem * getParent() const;
    void setParent(ComputeSystem * parent_);

    virtual Link * getLink(ComputeSystem * cs1, ComputeSystem * cs2) const = 0;

    virtual ComputeSystem * copy() const = 0;
};

#endif /* __COMPUTE_SYSTEM_HPP__ */

