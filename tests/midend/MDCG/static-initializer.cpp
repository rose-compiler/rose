
#include "MFB/Sage/driver.hpp"

#include "MDCG/model-builder.hpp"
#include "MDCG/code-generator.hpp"

#include "sage3basic.h"

#include <cassert>

struct PointModelTraversal {
  struct input_t {
    std::string name;
    unsigned x;
    unsigned y;
    unsigned z;
  };

  static SgExpression * createFieldInitializer(
    const MDCG::CodeGenerator & codegen,
    MDCG::Model::field_t element,
    unsigned field_id,
    const input_t & input,
    unsigned file_id
  ) {
    switch (field_id) {
      case 0:
        return SageBuilder::buildStringVal(input.name.c_str());
      case 1:
        return SageBuilder::buildAggregateInitializer(
                 SageBuilder::buildExprListExp(
                   SageBuilder::buildUnsignedIntVal(input.x),
                   SageBuilder::buildUnsignedIntVal(input.y),
                   SageBuilder::buildUnsignedIntVal(input.z)
                 )
               );
      default:
        assert(false);
    }
  }
};

struct LaticeDescModelTraversal {
  struct input_t {
    std::string name;
    unsigned red;
    unsigned green;
    unsigned blue;
  };

  static SgExpression * createFieldInitializer(
    const MDCG::CodeGenerator & codegen,
    MDCG::Model::field_t element,
    unsigned field_id,
    const input_t & input,
    unsigned file_id
  ) {
    switch (field_id) {
      case 0:
        return SageBuilder::buildStringVal(input.name.c_str());
      case 1:
        return SageBuilder::buildAggregateInitializer(
                 SageBuilder::buildExprListExp(
                   SageBuilder::buildUnsignedIntVal(input.red),
                   SageBuilder::buildUnsignedIntVal(input.green),
                   SageBuilder::buildUnsignedIntVal(input.blue)
                 )
               );
      default:
        assert(false);
    }
  }
};

struct LaticeModelTraversal {
  struct input_t {
    LaticeDescModelTraversal::input_t latice_desc;
    std::vector<PointModelTraversal::input_t> points;
  };

  static SgExpression * createFieldInitializer(
    const MDCG::CodeGenerator & codegen,
    MDCG::Model::field_t element,
    unsigned field_id,
    const input_t & input,
    unsigned file_id
  ) {
    switch (field_id) {
      case 0:
      {
        MDCG::Model::type_t type = element->node->type;
        assert(type != NULL && type->node->kind == MDCG::Model::node_t<MDCG::Model::e_model_type>::e_typedef_type);
        type = type->node->base_type;
        assert(type != NULL && type->node->kind == MDCG::Model::node_t<MDCG::Model::e_model_type>::e_pointer_type);
        type = type->node->base_type;
        assert(type != NULL && type->node->kind == MDCG::Model::node_t<MDCG::Model::e_model_type>::e_class_type);
        return codegen.createPointer<LaticeDescModelTraversal>(type->node->base_class, input.latice_desc, file_id);
      }
      case 1:
        return SageBuilder::buildUnsignedLongVal(input.points.size());
      case 2:
      {
        MDCG::Model::type_t type = element->node->type;
        assert(type != NULL && type->node->kind == MDCG::Model::node_t<MDCG::Model::e_model_type>::e_array_type);
        type = type->node->base_type;
        assert(type != NULL && type->node->kind == MDCG::Model::node_t<MDCG::Model::e_model_type>::e_class_type);
        return codegen.createArray<PointModelTraversal>(type->node->base_class, input.points.begin(), input.points.end(), file_id);
      }
      default:
        assert(false);
    }
  }
};

struct WorldModelTraversal {
  struct input_t {
    std::vector<LaticeModelTraversal::input_t> latices;
  };

  static SgExpression * createFieldInitializer(
    const MDCG::CodeGenerator & codegen,
    MDCG::Model::field_t element,
    unsigned field_id,
    const input_t & input,
    unsigned file_id
  ) {
    switch (field_id) {
      case 0:
        return SageBuilder::buildUnsignedLongVal(input.latices.size());
      case 1:
      {
        MDCG::Model::type_t type = element->node->type;
        assert(type != NULL && type->node->kind == MDCG::Model::node_t<MDCG::Model::e_model_type>::e_pointer_type);
        type = type->node->base_type;
        assert(type != NULL && type->node->kind == MDCG::Model::node_t<MDCG::Model::e_model_type>::e_typedef_type);
        type = type->node->base_type;
        assert(type != NULL && type->node->kind == MDCG::Model::node_t<MDCG::Model::e_model_type>::e_pointer_type);
        type = type->node->base_type;
        assert(type != NULL && type->node->kind == MDCG::Model::node_t<MDCG::Model::e_model_type>::e_class_type);
        return codegen.createArrayPointer<LaticeModelTraversal>(type->node->base_class, input.latices.size(), input.latices.begin(), input.latices.end(), file_id);
      }
      default:
        assert(false);
    }
  }
};

int main(int argc, char ** argv) {
  std::ofstream ofs;
  assert(argc == 2);

  SgProject * project = new SgProject();
  std::vector<std::string> arglist;
    arglist.push_back("c++");
    arglist.push_back("-DSKIP_ROSE_BUILTIN_DECLARATIONS");
    arglist.push_back("-c");
  project->set_originalCommandLineArgumentList (arglist);

  MFB::Driver<MFB::Sage> driver(project);

  unsigned output_file_id = driver.add(boost::filesystem::path(std::string(argv[1]) + "/gen_02.c"));
  driver.setUnparsedFile(output_file_id);

  MDCG::ModelBuilder model_builder(driver);
  MDCG::CodeGenerator codegen(driver);

  unsigned model = model_builder.create();

  model_builder.add(model, "test_02", argv[1], "h");

  std::set<MDCG::Model::class_t> classes;
  model_builder.get(model).lookup<MDCG::Model::class_t>("world_t_", classes);
  assert(classes.size() == 1);
  MDCG::Model::class_t world_class = *(classes.begin());

  PointModelTraversal::input_t l0p0 = {std::string("l0p0"), 0,0,0};
  PointModelTraversal::input_t l0p1 = {std::string("l0p1"), 0,0,0};
  PointModelTraversal::input_t l0p2 = {std::string("l0p2"), 0,0,0};
  PointModelTraversal::input_t l0p3 = {std::string("l0p3"), 0,0,0};
  PointModelTraversal::input_t l0p4 = {std::string("l0p4"), 0,0,0};
  LaticeDescModelTraversal::input_t l0_desc = {std::string("l0"), 0,0,0};
  LaticeModelTraversal::input_t l0;
    l0.latice_desc = l0_desc;
    l0.points.push_back(l0p0);
    l0.points.push_back(l0p1);
    l0.points.push_back(l0p2);
    l0.points.push_back(l0p3);
    l0.points.push_back(l0p4);
  
  PointModelTraversal::input_t l1p0 = {std::string("l1p0"), 0,0,0};
  PointModelTraversal::input_t l1p1 = {std::string("l1p1"), 0,0,0};
  PointModelTraversal::input_t l1p2 = {std::string("l1p2"), 0,0,0};
  LaticeDescModelTraversal::input_t l1_desc = {std::string("l1"), 0,0,0};
  LaticeModelTraversal::input_t l1;
    l1.latice_desc = l1_desc;
    l1.points.push_back(l1p0);
    l1.points.push_back(l1p1);
    l1.points.push_back(l1p2);

  PointModelTraversal::input_t l2p0 = {std::string("l2p0"), 0,0,0};
  PointModelTraversal::input_t l2p1 = {std::string("l2p1"), 0,0,0};
  PointModelTraversal::input_t l2p2 = {std::string("l2p2"), 0,0,0};
  PointModelTraversal::input_t l2p3 = {std::string("l2p3"), 0,0,0};
  PointModelTraversal::input_t l2p4 = {std::string("l2p4"), 0,0,0};
  PointModelTraversal::input_t l2p5 = {std::string("l2p5"), 0,0,0};
  LaticeDescModelTraversal::input_t l2_desc = {std::string("l2"), 0,0,0};
  LaticeModelTraversal::input_t l2;
    l2.latice_desc = l2_desc;
    l2.points.push_back(l2p0);
    l2.points.push_back(l2p1);
    l2.points.push_back(l2p2);
    l2.points.push_back(l2p3);
    l2.points.push_back(l2p4);
    l2.points.push_back(l2p5);

  WorldModelTraversal::input_t input;
  input.latices.push_back(l0);
  input.latices.push_back(l1);
  input.latices.push_back(l2);

  codegen.addDeclaration<WorldModelTraversal>(world_class, input, output_file_id, "my_world");

  project->unparse();

  return 0;
}

