
#ifndef ROSE_PROPOSED_MFB_JSON
#define ROSE_PROPOSED_MFB_JSON

#include "nlohmann/json.hpp"

#include <boost/filesystem.hpp>

namespace MFB {
  template <typename>
  class Sage;

  template <template <typename> class>
  class Driver;
}

namespace ROSE {
  using nlohmann::json;

  namespace proposed {
    namespace MFB {

      class Environment {
        public:
          ::MFB::Driver<::MFB::Sage> * driver;

        public:
          json config;

        protected:
          void parseConfig(int argc, char ** argv);
          void loadIncludeDirs();
          void configureRose(const json & rosecfg);
          void loadSource(const json & source);

          boost::filesystem::path resolvePath(boost::filesystem::path path) const;

        public:
          Environment(int argc, char ** argv);
          ~Environment();

        public:
          
      };

    }
  }
}

#endif

