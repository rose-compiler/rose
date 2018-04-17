#ifndef COBOL_BUILDER_INTERFACE_H
#define COBOL_BUILDER_INTERFACE_H

//-----------------------------------------------------------------------------------
// Abstract builder class for Cobol.
//-----------------------------------------------------------------------------------

namespace Cobol {

class CobolBuilderInterface
  {
    public:

    virtual std::string getCurrentFilename() = 0;

  };

} // namespace Cobol

// endif for COBOL_BUILDER_INTERFACE_H
#endif

