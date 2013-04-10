#ifndef __HCPRAGMA_H__
#define __HCPRAGMA_H__
/**
 * HC pragma support from ROSE
 * 
 * author yy8
 * yy8@rice.edu
 * 
 */

#include "rose.h"
#include <iostream>

using namespace SageInterface;
using namespace SageBuilder;

enum hcpragma_enum {
  pragma_hc_entry,
  pragma_hc_suspendable,
  pragma_hc_entry_suspendable,
  pragma_hc_suspendable_entry,
  pragma_CUDA_KERNEL,
  pragma_CUDA_AUTODIM,
  pragma_CUDA_DIM,
  pragma_hc_none
};


// Liao 4/8/2011, new AstAttribute to cover all hc pragmas
/*
---------- grammar begin -----------
% 'string' means literal string to be matched
% | means alternation
hcc_pragma = '#pragma' hc_simple_part | hc_cuda_part

hc_simple_part = 'hc' 'entry'| 'suspendable' | 'entry suspendable' | 'suspendable entry'

hc_cuda_part = 'CUDA' kernel_part| place_part

kernel_part = 'kernel'

% place could be an expression
% the grammar uses assignment_expression instead of expression to disallow comma expressions
% (list of expressions connected with comma) e.g. exp1, exp2 will be parsed to be (ex1, exp2) otherwise.
% 
place_part = assignment_expression autodim_part | dim_part

% autodim(<dim1>[, <dim2>, <dim3>, <shared_size>])
%  [ ] means optional
% , means  ',' to be simple
% assignment_expression is used to avoid parsing exp1, exp2, exp3 to be one single comma expression ((exp1,exp2),exp3)
autodim_part = 'autodim' '('  assignment_expression [, assignment_expression [, assignment_expression [, assignment_expression ] ] ]  ')'

% dim(blocksPerGrid, threadsPerBlock[, shared_size])
dim_part = 'dim' '('  assignment_expression ,  assignment_expression ,  [ , assignment_expression ]  ')'

---------------- grammar end -----------
 
 */ 

class HC_PragmaAttribute: public AstAttribute 
{
  public:
    SgNode * node; 
    enum hcpragma_enum pragma_type;

    HC_PragmaAttribute (SgNode* n , hcpragma_enum p_type): node(n), pragma_type(p_type) { }
    virtual SgExpression* get_dim1 (){ assert(0); return NULL; }; 
    virtual SgExpression* get_dim2 (){assert(0); return NULL; }; 
    virtual SgExpression* get_dim3 (){assert(0); return NULL; }; 

    virtual SgExpression* get_blocksPerGrid(){assert(0); return NULL; };
    virtual SgExpression* get_threadsPerGrid(){assert(0); return NULL; };
    virtual SgExpression* get_shared_size(){assert(0); return NULL; };

    virtual std::string toString ()
    {
      std::string result;
      result += "#pragma ";
      switch (pragma_type)
      {
        case pragma_hc_entry:
          result += "hc entry";
          break;
        case pragma_hc_suspendable:
          result += "hc suspendable";
          break;
        case pragma_hc_entry_suspendable:
          result += "hc entry suspendable";
          break;
        case pragma_hc_suspendable_entry:
          result += "hc suspendable entry";
          break;
        case pragma_CUDA_KERNEL:
          result += "CUDA kernel";
          break;
        case pragma_CUDA_AUTODIM:
          result += "CUDA";
          break;
        case pragma_CUDA_DIM:
          result += "CUDA";
          break;
        default:
          std::cerr<<"Error. HC_PragmaAttribute::toString(), illegal pragma type."<<std::endl;
          assert(false);
      }  
      return result; 
    }

}; 

class HC_CUDA_PragmaAttribute: public  HC_PragmaAttribute
{
  public:
    SgExpression* place_exp;
    HC_CUDA_PragmaAttribute (SgNode* n , hcpragma_enum p_type, SgExpression* pl):HC_PragmaAttribute(n,p_type), place_exp(pl) {} 
    virtual std::string toString()
    {
      std::string result = HC_PragmaAttribute::toString();

      result += " "+place_exp->unparseToString();

      if (pragma_type == pragma_CUDA_AUTODIM)
        result += " AUTODIM";
      else if (pragma_type == pragma_CUDA_DIM)
        result += " DIM";
      else 
      {
        std::cerr<<"wrong pragma type with a place expression!"<<std::endl;
        assert(false);  
      }


      return result;
    }
};

class HC_CUDA_autodim_PragmaAttribute: public HC_CUDA_PragmaAttribute
{
  public:
    SgExpression* dim1_exp;
    SgExpression* dim2_exp;
    SgExpression* dim3_exp;
    SgExpression* shared_size_exp;
    HC_CUDA_autodim_PragmaAttribute (SgNode* n , hcpragma_enum p_type, SgExpression* pl, 
          SgExpression* d1_e, SgExpression* d2_e, SgExpression* d3_e, SgExpression*sz_e): 
          HC_CUDA_PragmaAttribute (n, p_type, pl), dim1_exp(d1_e), dim2_exp(d2_e), dim3_exp(d3_e),shared_size_exp(sz_e) {}
  
  virtual std::string toString()
  {
    std::string result= HC_CUDA_PragmaAttribute::toString();
    result += " ("+ dim1_exp->unparseToString();
    if (dim2_exp !=NULL)
      result +=", "+ dim2_exp->unparseToString();
    if (dim3_exp !=NULL)
      result +=", "+ dim3_exp->unparseToString();
    if (shared_size_exp !=NULL)
      result +=", "+ shared_size_exp->unparseToString();
    result +=")";
    return result;
  }
  SgExpression* get_dim1 () {return dim1_exp;}
  SgExpression* get_dim2 () {return dim2_exp;}
  SgExpression* get_dim3 () {return dim3_exp;}
  SgExpression* get_shared_size() {return shared_size_exp;}
};

 class HC_CUDA_dim_PragmaAttribute: public HC_CUDA_PragmaAttribute
{
  public:
    SgExpression* block_exp;
    SgExpression* threads_exp;
    SgExpression* shared_size_exp;
    HC_CUDA_dim_PragmaAttribute (SgNode* n , hcpragma_enum p_type, SgExpression* pl, 
          SgExpression* block_e, SgExpression* threads_e, SgExpression*sz_e): 
          HC_CUDA_PragmaAttribute (n, p_type, pl), block_exp(block_e), threads_exp(threads_e), shared_size_exp(sz_e) {}
   virtual std::string toString()
  {
    std::string result= HC_CUDA_PragmaAttribute::toString();
    result += " ("+ block_exp->unparseToString();
    if (threads_exp!=NULL)
      result +=", "+ threads_exp->unparseToString();
    if (shared_size_exp !=NULL)
      result +=", "+ shared_size_exp->unparseToString();
    result +=")";
    return result;
  }
  SgExpression* get_blocksPerGrid() {return block_exp;}
  SgExpression* get_threadsPerGrid() {return threads_exp;}
  SgExpression* get_shared_size() {return shared_size_exp;}
 
};
 
extern AstAttribute* parse_HC_Pragma(SgPragmaDeclaration* pragmaDecl);

#endif /* __HCPRAGMA_H__ */
