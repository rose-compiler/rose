// Example MPI code generator
// Liao 9/22/2015
#include "rose.h"

#include "mpiCodeGenerator.h"

using namespace std;
using namespace AstFromString; // parser building blocks
using namespace SageInterface;
using namespace SageBuilder;
using namespace MPI_Code_Generator;

//! A test main program 
int main ( int argc, char * argv[] )
{
  std::vector <std::string> argvList (argv, argv + argc);
 
  // turn on OpenMP parsing support for device() map (a[][] dist_data()) 
  argvList.push_back("-rose:openmp:parse_only");

  SgProject* project = frontend(argvList); //frontendConstantFolding);
  ROSE_ASSERT(project != NULL);

  // Assuming single input file for this prototype
  ROSE_ASSERT (project->get_fileList().size() ==1);
  SgFile * cur_file = project->get_fileList()[0];

  SgSourceFile* sfile = isSgSourceFile(cur_file);
  
  setupMPIInit (sfile);
  setupMPIFinalize (sfile);

  std::vector <MPI_PragmaAttribute*> pragma_attribute_list;
  // Parsing all relevant pragmas, generate MPI_Pragma_Attribute_List.
  parsePragmas (sfile, pragma_attribute_list);
  translatePragmas (pragma_attribute_list);

  AstTests::runAllTests(project);
  return backend(project);

  return 0;
}

//! Implementation for the MPI code generator, including parsing pragrmas
//TODO: move to a separated file 
namespace MPI_Code_Generator 
{
  //! Default value is explicit, all code in main must be explicitly specified to be run by master or all processes.
  mpi_pragma_enum mpi_device_default_choice = e_semantics_explicit;

  //! Parsing all relevant pragmas, generate MPI_Pragma_Attribute_List.
  AstAttribute* parse_MPI_Pragma (SgPragmaDeclaration* pragmaDecl)
  {
    AstAttribute*  result = NULL;
    assert (pragmaDecl != NULL);
    assert (pragmaDecl->get_pragma() != NULL);
    string pragmaString = pragmaDecl->get_pragma()->get_pragma();

    // make sure it is side effect free
    const char* old_char = c_char;
    SgNode* old_node = c_sgnode;

    //c_sgnode = getNextStatement(pragmaDecl);
    c_sgnode = pragmaDecl;
    assert (c_sgnode != NULL);

    SgPragmaDeclaration* pdecl = isSgPragmaDeclaration (c_sgnode);
    assert (pdecl != NULL);

    c_char = pragmaString.c_str();

    if (afs_match_substr("omp"))
    {
      if (afs_match_substr("mpi_device_default"))
      {
        result = new MPI_PragmaAttribute (pdecl, pragma_mpi_device_default);
        assert (result != NULL);
      }
      else if (afs_match_substr("parallel for"))
      {
        result = new MPI_PragmaAttribute (pdecl, pragma_parallel_for);
        assert (result != NULL);
      } 
      else if (afs_match_substr("target"))
      {
        mpi_pragma_enum device_value; 
        //TODO : more fine grain parsing of device() clause
        if (afs_match_substr("device(mpi:all)"))
          device_value = e_mpi_all;
        else if (afs_match_substr("device(mpi:master)"))  
          device_value = e_mpi_master;
        else 
        {
          cerr<<"Error in parse_MPI_Pragma(), expecting mpi:all or mpi:master after target device("<<endl;
          assert (false); 
        }

        if (afs_match_substr("begin"))
        {
          if (device_value ==e_mpi_all )
          {
            result = new MPI_PragmaAttribute (pdecl, pragma_mpi_device_all_begin);
            assert (result != NULL);
          }
          else if (device_value ==e_mpi_master)
          {
            result = new MPI_PragmaAttribute (pdecl, pragma_mpi_device_master_begin);
            assert (result != NULL);
          }
        }  
        else if (afs_match_substr("end"))
        {
          if (device_value ==e_mpi_all )
          {
            result = new MPI_PragmaAttribute (pdecl, pragma_mpi_device_all_end);
            assert (result != NULL);
          }
          else if (device_value ==e_mpi_master)
          {
            result = new MPI_PragmaAttribute (pdecl, pragma_mpi_device_master_end);
            assert (result != NULL);
          }
        }
        else 
        { //TODO match map clauses

        } 
      }
    }
    // undo side effects
    c_char = old_char;
    c_sgnode = old_node;
    return result;

  } // end parse_MPI_Pragma()


  std::string MPI_PragmaAttribute::toString() 
  { 
    std::string result;
    result += "#pragma omp ";
    switch (pragma_type)
    { 
      case pragma_mpi_device_default:
        result += "mpi_device_default(explicit)"; //TODO: handle other options later
        break;
      case pragma_mpi_device_all_begin:
        result +="target device(mpi:all) begin";
        break;
      case pragma_mpi_device_all_end:
        result +="target device(mpi:all) end";
        break;
      case pragma_mpi_device_master_begin:
        result +="target device(mpi:master) begin";
        break;
      case pragma_mpi_device_master_end:
        result +="target device(mpi:master) end";
        break;
      case pragma_mpi_device_all_map_dist:
        result +="target device(mpi:all) map("; //TODO handle multiple map clauses
        break;
      case pragma_parallel_for:
        result +="parallel for";
        break;
      default:
        std::cerr<<"Error. MPI_PragmaAttribute::toString(), illegal pragma type."<<std::endl;
        assert(false);

    }
    return result;
  } //end toString


  void parsePragmas(SgSourceFile* sfile, std::vector <MPI_PragmaAttribute*>& MPI_Pragma_Attribute_List)
  {
    Rose_STL_Container<SgNode*> nodeList = NodeQuery::querySubTree(sfile,V_SgPragmaDeclaration);
    for (Rose_STL_Container<SgNode *>::iterator i = nodeList.begin(); i != nodeList.end(); i++)
    {
      //      cout << "Found a pragma!" << endl;
      SgPragmaDeclaration * pdecl = isSgPragmaDeclaration(*i); 
      ROSE_ASSERT (pdecl != NULL);
      AstAttribute* result = NULL;
      result = parse_MPI_Pragma (pdecl);
      MPI_PragmaAttribute* hc_att = dynamic_cast <MPI_PragmaAttribute*> (result);
      if (hc_att != NULL)
      {
        MPI_Pragma_Attribute_List.push_back (hc_att);  
        // debugging here
        cout<<hc_att->toString()<<endl;
      }
    } //end for

  } //end parsePragmas()


  void setupMPIInit(SgSourceFile* cur_file)
  {
    //#include "mpi.h" 
    SageInterface::insertHeader (cur_file, "libxomp_mpi.h", false);
    SageInterface::insertHeader (cur_file, "mpi.h", false);
    SgFunctionDeclaration* main_decl = findMain(cur_file);
   // TODO: handle multiple files, some of them don't have main()
    ROSE_ASSERT (main_decl != NULL);
    SgFunctionDefinition* main_def = main_decl->get_definition();
    ROSE_ASSERT (main_def != NULL);
    SgBasicBlock* func_body = main_def->get_body();
    ROSE_ASSERT (func_body != NULL);

    // Setup MPI
    SgStatement* decl_rank = buildStatementFromString("int _xomp_rank;", func_body);
    prependStatement(decl_rank, func_body);

    SgStatement* decl_nprocs= buildStatementFromString("int _xomp_nprocs;", func_body);
    prependStatement(decl_nprocs, func_body);

    // xomp_init_mpi (&argc, &argv, &_xomp_rank, &_xomp_nprocs);
    SgExprListExp * para_list = buildExprListExp (buildAddressOfOp (buildVarRefExp("argc", func_body)),
        buildAddressOfOp (buildVarRefExp("argv", func_body)),
        buildAddressOfOp (buildVarRefExp("_xomp_rank", func_body)),
        buildAddressOfOp (buildVarRefExp("_xomp_nprocs", func_body))
        );
    SgExprStatement* mpi_init_stmt = buildFunctionCallStmt ("xomp_init_mpi", buildIntType(), para_list, func_body);
    //     SgStatement* last_decl = findLastDeclarationStatement (func_body);
    insertStatementAfter (decl_rank, mpi_init_stmt);
  }

   //! Setup MPI finalize 
  void setupMPIFinalize(SgSourceFile* sfile)
  {
    SgFunctionDeclaration* main_decl = findMain(sfile);
    // TODO: handle multiple files, some of them don't have main()
    ROSE_ASSERT (main_decl != NULL);
    SgFunctionDefinition* main_def = main_decl->get_definition();
    ROSE_ASSERT (main_def != NULL);
    SgBasicBlock* func_body = main_def->get_body();
    ROSE_ASSERT (func_body != NULL);

    SgStatement* mf = buildFunctionCallStmt ("MPI_Finalize", buildVoidType(), NULL, func_body );
    
    instrumentEndOfFunction (main_decl, mf);
  }

  //! Translate generated Pragma Attributes one by one
  void translatePragmas (std::vector <MPI_PragmaAttribute*>& Attribute_List)
  {
    std::vector<MPI_PragmaAttribute*>::iterator iter;
    for (iter = Attribute_List.begin(); iter!=Attribute_List.end(); iter ++)
    {
      MPI_PragmaAttribute* cur_attr = *iter; 
      cout<<"Translating ..." << cur_attr->toString() <<endl;
      SgScopeStatement* scope = cur_attr->pragma_node ->get_scope();
      ROSE_ASSERT (scope != NULL);
      // simply obtain the default value and remove the pragma
      if (cur_attr-> pragma_type == pragma_mpi_device_default)
      {
        mpi_device_default_choice = cur_attr->default_semantics;
        // no automatic handling of attached preprocessed info. for now
        removeStatement(cur_attr->pragma_node, false);
      }
      // find omp target device(mpi:all) begin
      else if (cur_attr-> pragma_type == pragma_mpi_device_all_begin)
      {
        iter ++; // additional increment once
        MPI_PragmaAttribute* end_attribute = *iter; 
        ROSE_ASSERT (end_attribute->pragma_type = pragma_mpi_device_all_end);
        removeStatement(cur_attr->pragma_node, false);
        removeStatement(end_attribute ->pragma_node, false);
      }  
      else if (cur_attr-> pragma_type == pragma_mpi_device_master_begin)
      { // TODO refactor into a function
        iter ++; // additional increment once
        MPI_PragmaAttribute* end_attribute = *iter; 
        ROSE_ASSERT (end_attribute->pragma_type = pragma_mpi_device_master_end);

        //insert a if (rank) .. after the end pragma
        SgIfStmt * ifstmt = buildIfStmt (buildEqualityOp(buildVarRefExp("_xomp_rank", scope), buildIntVal(0)), buildBasicBlock(), NULL);
        insertStatementAfter (end_attribute->pragma_node, ifstmt);
        SgBasicBlock * bb = isSgBasicBlock(ifstmt->get_true_body());

        SgStatement* next_stmt = getNextStatement(cur_attr->pragma_node); // the next stmt is BB, skip it by starting the search from it
        ROSE_ASSERT (next_stmt != NULL);
        // normalize all declarations
        while ( next_stmt != end_attribute ->pragma_node)
        {
          // save current stmt before getting next one 
          SgStatement* cur_stmt = next_stmt; 
          next_stmt = getNextStatement (next_stmt);
          ROSE_ASSERT (next_stmt != NULL);

          if (SgVariableDeclaration* decl = isSgVariableDeclaration (cur_stmt))
            splitVariableDeclaration (decl);
        }
        // move all non-declaration statements in between into the block
        next_stmt = getNextStatement(cur_attr->pragma_node); //reset from the beginning
        while ( next_stmt != end_attribute ->pragma_node) 
        {
          // save current stmt before getting next one 
          SgStatement* cur_stmt = next_stmt; 
          next_stmt = getNextStatement (next_stmt);
          ROSE_ASSERT (next_stmt != NULL);
           
          if (!isSgVariableDeclaration(cur_stmt))
          {
            // now remove the current stmt
            removeStatement (cur_stmt, false);
            appendStatement(cur_stmt, bb);
          }
        }
        
        // remove pragmas
        removeStatement(cur_attr->pragma_node, false);
        removeStatement(end_attribute ->pragma_node, false);
      }  

    }  // end for
  } // end translatePragmas ()

} // end namespace
