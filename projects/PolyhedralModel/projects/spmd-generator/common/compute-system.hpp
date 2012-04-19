
#ifndef __COMPUTE_SYSTEM_HPP__
#define __COMPUTE_SYSTEM_HPP__

#include <vector>

class ComputeSystem {
  protected:
    unsigned id;

  static unsigned id_cnt;

  public:
    ComputeSystem();
    virtual ~ComputeSystem();

    unsigned getID() const;
};

class Link {
  protected:
    unsigned id;

  static unsigned id_cnt;

  public:
    Link();
    virtual ~Link();

    unsigned getID() const;
};

class System : public ComputeSystem {
  protected:
    std::vector<ComputeSystem *> elements;

  public:
    System();
    virtual ~System();

    virtual Link * getLink(ComputeSystem * cs1, ComputeSystem * cs2) = 0;
};

#endif /* __COMPUTE_SYSTEM_HPP__ */

