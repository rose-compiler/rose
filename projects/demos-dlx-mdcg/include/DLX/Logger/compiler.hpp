
#ifndef __DLX_COMPILER_LOGGER_HPP__
#define __DLX_COMPILER_LOGGER_HPP__

#include "MFB/Sage/driver.hpp"

#include "MDCG/model-builder.hpp"
#include "MDCG/static-initializer.hpp"

#include "MDCG/model.hpp"
#include "MDCG/model-class.hpp"
#include "MDCG/model-field.hpp"

#include <string>

namespace DLX {

namespace Logger {

struct compiler_modules_t {
  MFB::Driver<MFB::Sage> driver;
  MDCG::ModelBuilder model_builder;
  MDCG::StaticInitializer static_initializer;

  MFB::Driver<MFB::Sage>::file_id_t logger_data_file;

  struct api_t {
    SgFunctionSymbol * logger_log_before;
    SgFunctionSymbol * logger_log_after;

    MDCG::Model::class_t logger_data_class;
  } api;

  compiler_modules_t(SgProject * project, std::string logger_data_file_name, std::string liblogger_inc_dir);

private:
  void loadAPI(std::string & liblogger_inc_dir);
};

}

}

#endif /* __DLX_COMPILER_LOGGER_HPP__ */
