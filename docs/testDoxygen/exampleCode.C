#include "rose.h"
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif



/**
\brief This function creates a function and tries to insert and InitializeName variable into its parameter list. It fails to insert the variable. 
*/
void create_function_and_insert_arg(SgProject * projectNode)
   {
  // *****************************************************
  // Create the functionDeclaration 
  // *****************************************************
     SgType * __func_return_type           =   new SgTypeInt();

     Sg_File_Info * __file_info            =   new Sg_File_Info();

  // DQ: Must mark the newly built node to be a part of a transformation so that it will be unparsed!
  // __file_info->setTransformation();
     __file_info->set_isPartOfTransformation(true);

     SgName __func_name                    =   "rose_outlined_function";

     SgFunctionType * __func_type          =   new SgFunctionType(__func_return_type,false);

     SgFunctionDeclaration * __func        =   new SgFunctionDeclaration(__file_info, __func_name, __func_type);

     SgFunctionDefinition *  __func_def    =   new SgFunctionDefinition(__file_info, __func);

     SgBasicBlock         * __bb           =   new SgBasicBlock(__file_info);

  // Sets the body into the definition
     __func_def->set_body(__bb);
  // Sets the name
     __func->set_name(__func_name);
  // Sets the type
     __func->set_type(__func_type);
  // Sets the definition
     __func->set_definition(__func_def);
  // Sets the defintion's parent to the declaration
     __func_def->set_parent(__func);

  // ********************************************************
  // Create the InitializedName
  // *******************************************************
     SgName __var1_name = "alin_var";

     SgTypeShort * __var1_type = new SgTypeShort();

     SgInitializer * __var1_initializer = 0;

  // DQ (7/27/2004): In an investigation with Alin, the following code can only work if the declaration input is NULL
  // SgInitializedName *__var1_init_name=new SgInitializedName(__var1_name, __var1_type, __var1_initializer, __func->get_parameterList());

  // 42
  // Example of SgInitializedName
     SgInitializedName *__var1_init_name=new SgInitializedName(__var1_name, __var1_type, __var1_initializer, NULL);
  // 43

    // Done constructing the InitializedName variable

    // !!!!!!!!!!!!!!! This is the point where it crashes.
     __func->get_parameterList()->append_arg(__var1_init_name);

     printf ("Number of declarations (before) = %zu \n",projectNode->get_file(0).get_root()->get_declarations().size());

  // __func->set_parent(projectNode->get_file(0).get_root());
  // projectNode->get_file(0).get_root()->get_declarations().push_front(__func);
     projectNode->get_file(0).get_root()->prepend_declaration(__func);

     printf ("Number of declarations (after) = %zu \n",projectNode->get_file(0).get_root()->get_declarations().size());
}




int
main ( int argc, char * argv[] )
   {
  // Main Function for default example ROSE Preprocessor
  // This is an example of a preprocessor that can be built with ROSE

  // Build the project object (AST) which we will fill up with multiple files and use as a
  // handle for all processing of the AST(s) associated with one or more source files.
     SgProject* project = frontend(argc,argv);
     ROSE_ASSERT (project != NULL);

     create_function_and_insert_arg(project);

     AstPDFGeneration pdftest;
     pdftest.generateInputFiles(project);

     printf ("Calling the backend() \n");

     return backend(project);
   }


