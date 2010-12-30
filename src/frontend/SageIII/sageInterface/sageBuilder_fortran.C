// tps (01/14/2010) : Switching from rose.h to sage3
// test cases are put into tests/roseTests/astInterfaceTests
#include "sage3basic.h"
#include "roseAdapter.h"
#include "markLhsValues.h"
#include "sageBuilder.h"
#include <fstream>
#include <boost/algorithm/string/trim.hpp>
#include "Outliner.hh"
using namespace std;
using namespace SageInterface;

//! Put Fortran-specific builders here
// Many of them share the same implementations as those in 
// src/frontend/OpenFortranParser_SAGE_Connection/fortran_support.C
//
// Liao 12/6/2010

SgAttributeSpecificationStatement * 
SageBuilder::buildAttributeSpecificationStatement(SgAttributeSpecificationStatement::attribute_spec_enum kind)
{
  SgAttributeSpecificationStatement *attributeSpecificationStatement = new SgAttributeSpecificationStatement();
  ROSE_ASSERT(attributeSpecificationStatement != NULL);

  attributeSpecificationStatement->set_definingDeclaration(attributeSpecificationStatement);
  attributeSpecificationStatement->set_firstNondefiningDeclaration(attributeSpecificationStatement);

  attributeSpecificationStatement->set_attribute_kind(kind);

  switch (kind)
  {
    case SgAttributeSpecificationStatement::e_parameterStatement:
    case SgAttributeSpecificationStatement::e_externalStatement:
    case SgAttributeSpecificationStatement::e_dimensionStatement:
    case SgAttributeSpecificationStatement::e_allocatableStatement:
      {
        SgExprListExp* parameterList = new SgExprListExp();
        attributeSpecificationStatement->set_parameter_list(parameterList);
        parameterList->set_parent(attributeSpecificationStatement);
        setSourcePositionForTransformation(parameterList);
        break;
      }
    default:
      cerr<<"SageBuilder::buildAttributeSpecificationStatement(), unhandled attribute specification kind:"<<kind <<endl;
      ROSE_ASSERT (false);
      break;
  }
  setSourcePositionForTransformation(attributeSpecificationStatement); 
  return attributeSpecificationStatement;
}

//! Build Fortran include line
SgFortranIncludeLine* 
SageBuilder::buildFortranIncludeLine(std::string filename)
{
  SgFortranIncludeLine* result = new SgFortranIncludeLine(filename);;
  ROSE_ASSERT (result != NULL);
  result->set_definingDeclaration(result);
  result->set_firstNondefiningDeclaration(result);
  setSourcePositionForTransformation (result);
  return result;
}
//! Build a Fortran common block, possibly with a name
SgCommonBlockObject* 
SageBuilder::buildCommonBlockObject(std::string name/*="" */, SgExprListExp* exp_list/*=NULL*/)
{
  SgCommonBlockObject* result = new SgCommonBlockObject();
  ROSE_ASSERT (result != NULL);
  
  result->set_block_name(name);

  if (exp_list != NULL)
  {
    result->set_variable_reference_list(exp_list);
    exp_list->set_parent(result);
  }
  setSourcePositionForTransformation (result);
  return result;
}

//! Build a Fortran Common statement
SgCommonBlock* 
SageBuilder::buildCommonBlock(SgCommonBlockObject* first_block/*=NULL*/)
{
  SgCommonBlock* result = new SgCommonBlock();
  ROSE_ASSERT (result != NULL);

  if (first_block != NULL)
  {
    result->get_block_list().push_back(first_block);
    first_block->set_parent(result);
  }
  
  result->set_definingDeclaration(result);
  result->set_firstNondefiningDeclaration(result);

  setSourcePositionForTransformation(result);
  return result;
}

