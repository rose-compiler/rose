/** 
 * \file MDCG/include/MDCG/model-builder.hpp
 *
 * \author Tristan Vanderbruggen
 *
 */

#ifndef __MDCG_MODEL_BUILDER_HPP__
#define __MDCG_MODEL_BUILDER_HPP__

#include "MDCG/handle-builder.hpp"

#include <map>

namespace MFB {
  template <class Model> class Driver;

  class Sage;

  struct api_t;
};

namespace MDCG {

/*!
 * \addtogroup grp_mdcg_model
 * @{
*/

class ModelBuilder :
  public Handles::handle_hook_t<ModelBuilder, build_model_t>,
  public Handles::handle_hook_t<ModelBuilder, get_model_t>
{
  public:
    static Model::model_id_t cnt_model_id;
    
  private:
    MFB::Driver<MFB::Sage> & p_mfb_driver;

    std::map<Model::model_id_t, Model::Model> p_models;

  private:
    const MFB::api_t & getApi(const std::string &) const;
    Model::model_id_t add(Model::model_id_t model, const MFB::api_t & api);

  public:
    ModelBuilder(MFB::Driver<MFB::Sage> & mfb_driver);

    Model::model_id_t build(const std::vector<std::string> & files);
    Model::model_id_t add(Model::model_id_t model, std::vector<std::pair<std::string, bool> > & files);

    const Model::Model & get(const Model::model_id_t model_id) const;
};

/** @} */

}

#endif /* __MDCG_MODEL_BUILDER_HPP__ */

