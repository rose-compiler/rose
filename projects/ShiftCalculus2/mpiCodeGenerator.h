/*
 *  
 *
 *  V 0.2 using real frontend parser and dedicated OpenMP-like AST nodes for program representation
 *  This is necessary to parse complex extended map clause with dist_data info.
 *  The previous version's MPI_PragmaAttribute is no longer used. 
 *
 *  Liao 12/11/2015
 *
 *  V 0.1
 *  Parsing pragmas and generating MPI code from input sequential code
 *  Pragma is OpenMP style, reusing OmpAttribute to store information
 *  As a experiments, a lightweight recursive descendent parser is used to parse the pragmas
 *  Liao 9/22/2015
 * */
#ifndef MPI_Code_Generator_h 
#define MPI_Code_Generator_h

#include <vector>
#include <string>
namespace MPI_Code_Generator 
{

//------------  v 0.2 interface, expecting the extended ROSE frontend to parse and create OpenMP AST nodes
//    using -rose:openmp:ast_only command line option to active the frontend support

  void lower_xomp (SgSourceFile* file);

  // Translate target device(mpi:master) begin ...
  void transMPIDeviceMaster (SgOmpTargetStatement * t_stmt);

//--------------- v 0.1 interface, no longer being used.   
  class MPI_PragmaAttribute; 
  //int generateMPI (SgSourceFile* sfile); 

  //! A prototype parser for directives guiding MPI code generation
  void parsePragmas(SgSourceFile* sfile, std::vector <MPI_PragmaAttribute*>& MPI_Pragma_Attribute_List);
   
  //! Translate generated Pragma Attributes
  void translatePragmas (std::vector <MPI_PragmaAttribute*>& MPI_Pragma_Attribute_List);  

  //! Setup MPI initialization
  void setupMPIInit(SgSourceFile* sfile);

  //! Setup MPI finalize 
  void setupMPIFinalize(SgSourceFile* sfile);

  // pragma enum values. 
  // For quick prototyping, we use AstAttributes instead of dedicated AST nodes for storing parsed results.
  enum mpi_pragma_enum {
  // for main function, what is the default semantics for code if no directives are present ? 
  //  run by all processes (spmd) vs. run only by master process, or must be explicitly declared ( device (mpi:all))
  //  #pragma omp mpi_device_default(mpi:all|mpi:master|explicit) 
    e_mpi_all, 
    e_mpi_master, 
    e_semantics_explicit, 
    
    //#pragma omp mpi_device_default(mpi:all|mpi:master|explicit)
    pragma_mpi_device_default,

    //#pragma omp target device(mpi:all) begin 
    pragma_mpi_device_all_begin,
    //#pragma omp target device(mpi:all) end
    pragma_mpi_device_all_end,

    // #pragma omp target device(mpi:master) begin
    pragma_mpi_device_master_begin,
    // #pragma omp target device(mpi:master) end
    pragma_mpi_device_master_end,

    // pragma omp target device(mpi:all) map ( dist_data)
    pragma_mpi_device_all_map_dist, 

    //#pragma omp parallel for
    pragma_parallel_for,
    pragma_last
  };

  // Global settings for the code generation
  extern mpi_pragma_enum mpi_device_default_choice; 

  class MPI_PragmaAttribute: public AstAttribute
  {
    public:
      SgPragmaDeclaration* pragma_node; // the associated AST node for pragma
      enum mpi_pragma_enum pragma_type; 
      enum mpi_pragma_enum default_semantics; 

      MPI_PragmaAttribute (SgPragmaDeclaration* n , mpi_pragma_enum p_type): pragma_node(n), pragma_type(p_type) 
    { default_semantics = e_semantics_explicit;  }

      // convert the attribute back to string format 
      std::string toString(); 

  }; // end class

  // parse a single pragma declaration, internal use only
  extern AstAttribute* parse_MPI_Pragma (SgPragmaDeclaration* pragmaDecl);

 // parse pragmas in an input file
 void parsePragmas(SgSourceFile* sfile);

} // end namespace


#endif //MPI_Code_Generator_h
