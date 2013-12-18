/** 
 * \file MDCG/include/MDCG/model-builder.hpp
 *
 * \author Tristan Vanderbruggen
 *
 */

#ifndef __MDCG_MODEL_BUILDER_HPP__
#define __MDCG_MODEL_BUILDER_HPP__

#include "MDCG/model.hpp"

//#include "MDCG/handle-builder.hpp"

#include <map>
#include <vector>
#include <iostream>

namespace MFB {
  template <template <typename T> class Model> class Driver;

  template <typename Object> class Sage;

  struct api_t;
};

namespace MDCG {

/*!
 * \addtogroup grp_mdcg_model
 * @{
*/

class ModelBuilder /*:
  public Handles::handle_hook_t<ModelBuilder, build_model_t>,
  public Handles::handle_hook_t<ModelBuilder, get_model_t>*/
{
  private:
    MFB::Driver<MFB::Sage> & p_mfb_driver;

    std::vector<Model::model_t> p_models;

  private:
    const MFB::api_t & getApi(const std::string &) const;
    void add(unsigned model, const MFB::api_t * api);

  public:
    ModelBuilder(MFB::Driver<MFB::Sage> & mfb_driver);

    unsigned create();

    void addOne(
      unsigned model,
      const std::string & name,
      const std::string & path,
      std::string suffix
    );

    void addPair(
      unsigned model,
      const std::string & name,
      const std::string & header_path,
      const std::string & source_path,
      std::string header_suffix,
      std::string source_suffix
    );

    const Model::model_t & get(unsigned model) const;

    void print(std::ostream & out, unsigned model) const;
};

/** @} */

}

#endif /* __MDCG_MODEL_BUILDER_HPP__ */

