// CoderInfo-ootm.ii:7:3: error: prior declaration of `NoiseType' at CoderInfo-ootm.ii:6:9 refers to a different entity, so it conflicts with the one being declared here

namespace MagickLib {
  typedef enum {ReadMode, WriteMode, IOMode} NoiseType;
  //typedef int NoiseType;    // same effect
}

namespace Magick {
  using MagickLib::NoiseType;
  using MagickLib::NoiseType;
}
