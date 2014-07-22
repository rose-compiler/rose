Model Driven Code Generator (MDCG)
==================================

MDCG is built on top of [MFB](../MFB/README.md).
It builds models of MFB's APIs.
These models can be traverse to drive code generation.

Generators:
 * Static Initializer
     *  input: MDCG model, data-structure
     * output: variable initializer
     * This generator traverses the *model* of a type that can store the content of *data-structure*.
       It produces at least one variable initializer (maybe more recursively) which contains the content of *data-structure*.


## Static Initializer

Given nested types:
```c
struct A {
  char c;
  int v;
};

struct B {
  struct A a;
  int n;
  struct A * arr;
};
```
And some data to be stored in this variable of this type:
```c++
std::pair<char, int> base;
std::vector<std::pair<char, int> > vector;
```
Static Initializer can store these data using:
```c++
struct ModelA {
  typedef std::pair<char, int> input_t;

  static SgExpression * createFieldInitializer(
    const MDCG::CodeGenerator & codegen,
    MDCG::Model::field_t element,
    unsigned field_id,
    const input_t & input,
    unsigned file_id
  ) {
    switch (field_id) {
      case 0:
        /// char c;
        return SageBuilder::buildCharVal(input.first);
      case 1:
        /// int v;
        return SageBuilder::buildIntVal(input.second);
      default:
        assert(false);
    }
  }
};

struct ModelB {
  struct input_t {
    std::pair<char, int> base;
    std::vector<std::pair<char, int> > vector;
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
        /// struct A a;
        MDCG::Model::type_t type = element->node->type;
        assert(type != NULL && type->node->kind == MDCG::Model::node_t<MDCG::Model::e_model_type>::e_class_type);
        return codegen.createInitializer<ModelA>(
                 type->node->base_class,
                 input.base,
                 file_id
        );
      case 1:
        /// int n;
        return SageBuilder::buildIntVal(input.vector.size());
      case 2:
        /// struct A * arr;
        MDCG::Model::type_t type = element->node->type;
        assert(type != NULL && type->node->kind == MDCG::Model::node_t<MDCG::Model::e_model_type>::e_pointer_type);
        type = type->node->base_type;
        assert(type != NULL && type->node->kind == MDCG::Model::node_t<MDCG::Model::e_model_type>::e_class_type);
        return codegen.createArrayPointer<ModelA>(
                 type->node->base_class,
                 input.vector.size(),
                 input.vector.begin(),
                 input.vector.end(),
                 file_id,
                 "vector"
               );
      default:
        assert(false);
    }
  }
};
```
Now let's consider:
```c++
int main() {
  SgProject * project = new SgProject();
  MFB::Driver<MFB::Sage> mfb_driver(project);

  size_t file_id = driver.create(boost::filesystem::path("./output.c"));
    driver.setUnparsedFile(file_id);
    driver.setCompiledFile(file_id);

  MDCG::ModelBuilder model_builder(mfb_driver);

  unsigned model_id = model_builder.create();
    model_builder.add(model_id, "input", ".", "h"); // load "input.h" from "." directory
    // can add as many files as needed

  const MDCG::Model::model_t & model = model_builder.get(model_id);

  MDCG::Model::class_t structB = model.lookup<MDCG::Model::class_t>("B");

  ModelB::input_t input;
  input.base = std::pair<char, int>('c', 69);
  input.vector.resize(3);
    input.vector[0] = std::pair<char, int>('d', 54);
    input.vector[1] = std::pair<char, int>('g', 96);
    input.vector[2] = std::pair<char, int>('w', 84);

  codegen.addDeclaration<ModelB>(structB, input, file_id, "output");

  return backend(project);
}
```
output.c:
```c
struct A vector[3] = {
  {'d',54},
  {'g',96},
  {'w',84}
};
struct B output = {
  {'c', 69},
  3,
  vector
};
```

