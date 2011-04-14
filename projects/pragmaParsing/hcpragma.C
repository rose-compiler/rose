/**
 * HC pragma support from ROSE
 * 
 * author yy8, liao
 * yy8@rice.edu, "Chunhua (Liao) Liao" <liao6@llnl.gov>
 * 
 */
#include "hcpragma.h"
#include "AstFromString.h"
#include <string>
using namespace SageInterface;
using namespace SageBuilder;
using namespace AstFromString;

  /*
     % autodim(<dim1>[, <dim2>, <dim3>, <shared_size>])
     %  [ ] means optional
     % , means  ',' to be simple
     autodim_part = 'autodim' '('  expression [, expression [, expression [, expression ] ] ]  ')'
     autodim_part_2 = '('  expression [, expression [, expression [, expression ] ] ]  ')'
     */
  bool afs_match_autodim_part_2 (SgExpression**e1, SgExpression ** e2, SgExpression ** e3, SgExpression** e4)
  {
    bool result = false;
    //i = pragma_CUDA_AUTODIM;

    if (!afs_match_char('('))
    {
      printf ("Error: expecting ( after parsing #pragma CUDA pl autodim\n");
      assert (false);
    }
    // mandatory dim1
    if (!afs_match_expression())
    {
      printf ("Error: expecting dim1 after parsing #pragma CUDA pl autodim(\n");
      assert (false);
    }
    assert (c_parsed_node != NULL);
    assert (isSgExpression(c_parsed_node) != NULL);
    *e1 = isSgExpression(c_parsed_node);

    //optional dim2
    if (afs_match_char(','))
    {
      if (!afs_match_expression())
      {
        printf ("Error: expecting dim2 after parsing #pragma CUDA pl autodim(dim1,\n");
        assert (false);
      }
      assert (c_parsed_node != NULL);
      assert (isSgExpression(c_parsed_node) != NULL);
      *e2= isSgExpression(c_parsed_node);

      //optional dim3
      if (afs_match_char(','))
      {
        if (!afs_match_expression())
        {
          printf ("Error: expecting dim3 after parsing #pragma CUDA pl autodim(dim1,dim2,\n");
          assert (false);
        }
        assert (c_parsed_node != NULL);
        assert (isSgExpression(c_parsed_node) != NULL);
        *e3= isSgExpression(c_parsed_node);
        // optional shared_size
        if (afs_match_char(','))
        {
          if (!afs_match_expression())
          {
            printf ("Error: expecting shared_size after parsing #pragma CUDA pl autodim(dim1,dim2,dim3,\n");
            assert (false);
          }
          assert (c_parsed_node != NULL);
          assert (isSgExpression(c_parsed_node) != NULL);
          *e4= isSgExpression(c_parsed_node);
        } // end if shared_size 
      } // end if dim3  
    } // end if dim2 

    if (!afs_match_char(')'))
    {
      printf ("Error: expecting ) after parsing #pragma CUDA pl autodim(...\n");
      assert (false);
    }
    return result;
  }

  /*
   * % dim(blocksPerGrid, threadsPerBlock[, shared_size])
   * dim_part = 'dim' '('  expression ,  expression ,  [ , expression ]  ')'
   * dim_part_2 = '('  expression ,  expression ,  [ , expression ]  ')'
   */
  bool afs_match_dim_part_2(SgExpression** e1, SgExpression** e2, SgExpression ** e3)
  {
    bool result = false;
        
    if (!afs_match_char('('))
    {
      printf ("Error: expecting ( after parsing #pragma CUDA pl dim\n");
      assert (false);
    }  
    // mandatory blocksPerGrid
    if (!afs_match_expression())
    {
      printf ("Error: expecting blocksPerGrid after parsing #pragma CUDA pl dim(\n");
      assert (false);
    }
    assert (c_parsed_node != NULL);
    assert (isSgExpression(c_parsed_node) != NULL);
    *e1 = isSgExpression(c_parsed_node);

    // mandatory threadsPerBlock_exp
    if (!afs_match_char(','))
    {
      printf ("Error: expecting , after parsing #pragma CUDA pl dim(blocksPerGrid\n");
      assert (false);
    }  

    if (!afs_match_expression())
    {
      printf ("Error: expecting threadsPerBlock_after parsing #pragma CUDA pl dim(blocksPerGrid, \n");
      assert (false);
    }
    assert (c_parsed_node != NULL);
    assert (isSgExpression(c_parsed_node) != NULL);
    *e2 = isSgExpression(c_parsed_node);

    if (afs_match_char(','))
    {
      if (!afs_match_expression())
      {
        printf ("Error: expecting shared_size after parsing #pragma CUDA pl dim(blocksPerGrid,threadsPerBlock,\n");
        assert (false);
      }
      assert (c_parsed_node != NULL);
      assert (isSgExpression(c_parsed_node) != NULL);
      *e3 = isSgExpression(c_parsed_node);

    } // end if shared_size

    if (!afs_match_char(')'))
    {
      printf ("Error: expecting ) after parsing #pragma CUDA pl dim(...\n");
      assert (false);
    }

    return result;
  }

AstAttribute* parse_HC_Pragma(SgPragmaDeclaration* pragmaDecl)
{
  AstAttribute*  result = NULL;
  assert (pragmaDecl != NULL);
  assert (pragmaDecl->get_pragma() != NULL);
  string pragmaString = pragmaDecl->get_pragma()->get_pragma();
  // make sure it is side effect free
  const char* old_char = c_char;
  SgNode* old_node = c_sgnode;

  c_sgnode = getNextStatement(pragmaDecl);
  assert (c_sgnode != NULL);

  c_char = pragmaString.c_str();

  if (afs_match_substr("hc")) 
  { 
    if (afs_match_substr("entry"))
    { 
      if (afs_match_substr("suspendable"))
      {
        //  printf("found hc entry suspendable!\n");
        result = new HC_PragmaAttribute(c_sgnode, pragma_hc_entry_suspendable);
        assert (result != NULL);
      } else
      {
        //  printf("found hc entry!\n");
        result = new HC_PragmaAttribute(c_sgnode, pragma_hc_entry);
        assert (result != NULL);
      }
    } else 
      if (afs_match_substr("suspendable"))
      {
        if (afs_match_substr("entry"))
        {
          //   printf("found hc suspendable entry!\n");
          result = new HC_PragmaAttribute(c_sgnode, pragma_hc_suspendable_entry);
          assert (result != NULL);
        }
        else
        {
          //   printf("found hc suspendable!\n");
          result = new HC_PragmaAttribute(c_sgnode, pragma_hc_suspendable);
          assert (result != NULL);
        }
      } 
  } 
  // pragmas starting with CUDA    
  else if (afs_match_substr("CUDA"))
  {
   // #pragma CUDA kernel
    if (afs_match_substr("kernel"))
    {
      //printf ("found CUDA kernel\n");
      result = new HC_PragmaAttribute(c_sgnode, pragma_CUDA_KERNEL);
      assert (result != NULL);
    }
    else if (afs_match_expression()) // place expression
    { 
      assert (c_parsed_node != NULL);
      assert (isSgExpression(c_parsed_node) != NULL);
      SgExpression* pl_exp = isSgExpression(c_parsed_node); // save the parsed place expression
      // #pragma CUDA <pl> autodim(<dim1>[, <dim2>, <dim3>, <shared_size>])
      if (afs_match_substr("autodim"))
      {
        //i = pragma_CUDA_AUTODIM;

        SgExpression* dim1_exp = NULL;
        SgExpression* dim2_exp = NULL, * dim3_exp= NULL, *sz_exp=NULL;
        afs_match_autodim_part_2 (&dim1_exp, &dim2_exp, &dim3_exp, &sz_exp);
        result = new HC_CUDA_autodim_PragmaAttribute (c_sgnode, pragma_CUDA_AUTODIM, pl_exp, dim1_exp, dim2_exp, dim3_exp,sz_exp ) ;
        assert (result != NULL);
      } // end autodim
      // #pragma CUDA <pl> dim(blocksPerGrid, threadsPerBlock[, shared_size])
      else if (afs_match_substr("dim"))
      {
        SgExpression* blocksPerGrid_exp= NULL;
        SgExpression* threadsPerBlock_exp = NULL;
        SgExpression* sz_exp= NULL;
        afs_match_dim_part_2 (&blocksPerGrid_exp, & threadsPerBlock_exp, &sz_exp);
        result = new HC_CUDA_dim_PragmaAttribute (c_sgnode, pragma_CUDA_DIM, pl_exp, blocksPerGrid_exp, threadsPerBlock_exp, sz_exp ) ;
        assert (result != NULL);
      } // end dim
      else 
      {
        printf ("error. Unrecoginized directive starting from CUDA pl \n");
        abort();
      }
    } // end if place part
    else // None of CUDA kernel or CUDA place_expression
    {
      printf("error. Unrecoginized directive starting from CUDA\n");
      assert (false);
    }
  } 

  // undo side effects
  c_char = old_char;
  c_sgnode = old_node;
  return result;
}

class visitorTraversal : public AstSimpleProcessing
{
  protected:
    virtual void visit(SgNode* n);
};

void visitorTraversal::visit(SgNode* node)
{
  if (node->variantT() == V_SgPragmaDeclaration) {
//      cout << "Found a pragma!" << endl;

      SgPragma * pragma =  isSgPragmaDeclaration(node)->get_pragma();
      string input_str = pragma->get_pragma();
      AstAttribute* result = NULL;
      result = parse_HC_Pragma (isSgPragmaDeclaration(node));
      HC_PragmaAttribute* hc_att = dynamic_cast <HC_PragmaAttribute*> (result);
      assert (hc_att != NULL);
      cout<<hc_att->toString()<<endl;
  }
}

int main(int argc, char * argv[])
{
  SgProject *project = frontend (argc, argv);
  visitorTraversal myvisitor;
  myvisitor.traverseInputFiles(project,preorder);

  return backend(project);
}

