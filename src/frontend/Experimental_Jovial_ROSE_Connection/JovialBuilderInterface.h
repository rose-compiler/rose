#ifndef JOVIAL_BUILDER_INTERFACE_H
#define JOVIAL_BUILDER_INTERFACE_H

//-----------------------------------------------------------------------------------
// Abstract builder class for Jovial.
//-----------------------------------------------------------------------------------

namespace Jovial {

class JovialBuilderInterface
  {
    public:

    virtual std::string getCurrentFilename() = 0;

  };

} // namespace Jovial

// endif for JOVIAL_BUILDER_INTERFACE_H
#endif

