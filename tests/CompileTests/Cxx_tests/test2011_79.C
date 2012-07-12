
// This test code demonstrates a problem with the name qualification of enum values
namespace SlipSurface
   {
     enum Surface
        { None,
          Type1,
          Type2,
          Type3,
        };
   }

void foobar()
   {
  // In the unparsed code, Type1 is output without its name qualification
     SlipSurface::Surface x = SlipSurface::Type1;
   }
