
#ifndef TYPEFORGE_TYPECHAIN_H
#  define TYPEFORGE_TYPECHAIN_H

#include <string>
#include <ostream>

class SgProject;

namespace Typeforge {

class Typechain {
  public:
    // TODO public types: enums?

  private:
    // TODO private types: templated capsules

  private:
    // TODO storage: maps, vectors, sets, ...

  private:
    // TODO static factory facilities

  private:
    // TODO private construction methods

  public:
    void initialize(SgProject * p = nullptr);

    // TODO public accessors

    void toDot(std::string const & fname) const;
    void toDot(std::ostream & out) const;
};

}

#endif /* TYPEFORGE_TYPECHAIN_H */

