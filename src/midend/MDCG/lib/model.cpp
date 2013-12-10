
#include "MDCG/model.hpp"

namespace MDCG {

namespace Model {

model_t::model_t() :
  variables(),
  functions(),
  types(),
  classes(),
  fields(),
  methods(),
  namespaces()
{}

model_t::model_t(const model_t & model) :
  variables(),
  functions(),
  types(),
  classes(),
  fields(),
  methods(),
  namespaces()
{
  /// \todo copy
}

Model::Model() :
  model()
{}

Model::Model(const model_t & model_) :
  model(model_)
{}

Model::Model(const Model & model_) :
  model(model_.model)
{}

Model::~Model() {}

const model_t & Model::getModel() const {
  return model;
}

}

}

