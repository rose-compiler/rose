/* unparser.C
 * Contains the implementation of the constructors, destructor, formatting functions,
 * and fucntions that unparse directives.
 */

#include "rose.h"

// include "array_class_interface.h"
#include "unparser.h"

#define __STDC_FORMAT_MACROS
#include <inttypes.h>

// DQ (12/31/2005): This is OK if not declared in a header file
using namespace std;

// extern ROSEAttributesList *getPreprocessorDirectives( char *fileName); // [DT] 3/16/2000

//-----------------------------------------------------------------------------------
//  Unparser::Unparser
//  
//  Constructor that takes a SgFile*, iostream*, ROSEAttributesList*, Unparser_Opt,
//  and int. All other fields are set to NULL or 0.
//-----------------------------------------------------------------------------------
// [DT] Old version: Unparser::Unparser(SgFile* nfile, iostream* nos, 
//                         ROSEAttributesList* nlist, Unparser_Opt nopt, int nline) {
// Unparser::Unparser(SgFile* nfile, ostream* nos, char *fname, 
//                    ROSEAttributesList* nlist, 
//                    ROSEAttributesListContainer* nlistList, 
//                    Unparser_Opt nopt, int nline)
// Unparser::Unparser(SgFile* nfile, ostream* nos, char *fname,
//                    Unparser_Opt nopt, int nline)
// Unparser::Unparser( ostream* nos, char *fname, Unparser_Opt nopt, int nline,
//                    UnparseFormatHelp *h, UnparseDelegate* r)

// DQ (8/19/2007): I have removed the "int nline" function parameter becuase it was part of an old mechanism
// to unparse a specific line, now replaced by the unparseToString() member function on each IR node.
// Unparser::Unparser( ostream* nos, string fname, Unparser_Opt nopt, int nline, UnparseFormatHelp *h, UnparseDelegate* r)
Unparser::Unparser( ostream* nos, string fname, Unparser_Opt nopt, UnparseFormatHelp *h, UnparseDelegate* r)
   : cur(nos, h), repl(r)
   {
     u_type     = new Unparse_Type(this);
     u_name     = new Unparser_Nameq(this);
  // u_support  = new Unparse_Support(this);
     u_sym      = new Unparse_Sym(this);
     u_debug    = new Unparse_Debug(this);
     u_sage     = new Unparse_MOD_SAGE(this);
     u_exprStmt = new Unparse_ExprStmt(this, fname);

  // DQ (8/14/2007): I have added this here to be consistant, but I question if this is a good design!
  // UnparseFortran_type* u_fortran_type;
  // FortranCodeGeneration_locatedNode* u_fortran_locatedNode;
     u_fortran_type = new UnparseFortran_type(this);
     u_fortran_locatedNode = new FortranCodeGeneration_locatedNode(this, fname);

  // ROSE_ASSERT(nfile != NULL);
     ROSE_ASSERT(nos != NULL);

  // ROSE_ASSERT(nlist != NULL);
  // file       = nfile;
  //   primary_os = nos; // [DT] 3/9/2000
  //
  // [DT] 3/17/2000 -- Should be careful here.  fname could include a path,
  //      and I think currentOutputFileName should be in the local directory,
  //      or a subdirectory of the local directory, perhaps.
  //
  //   strcpy(primaryOutputFileName, fname);
  // strcpy(currentOutputFileName, fname.c_str());


  // dir_list         = nlist;
  // dir_listList     = nlistList;
     opt              = nopt;

  // MK: If overload option is set true, the keyword "operator" occurs in the output.
  // Usually, that's not what you want, but it can be used to avoid a current bug,
  // see file TODO_MK. The default is to set this flag to false, see file
  // preprocessorSupport.C in the src directory
  // opt.set_overload_opt(true);

     cur_index        = 0;

     currentFile      = NULL;

  // DQ (8/19/2007): Removed this old unpasing mechanism.
  // line_to_unparse  = nline;
  // ltu  = nline;
   }

//-----------------------------------------------------------------------------------
//  Unparser::~Unparser
//
//  Destructor
//-----------------------------------------------------------------------------------
Unparser::~Unparser()
   {
     delete u_type;
     delete u_name;
     delete u_sym;
     delete u_debug;
     delete u_sage;
     delete u_exprStmt;
     delete u_fortran_type;
     delete u_fortran_locatedNode;
   }

UnparseFormat& Unparser::get_output_stream()
   {
     return cur;
   }




bool
Unparser::isPartOfTransformation( SgLocatedNode *n)
   {
  // return (n->get_file_info()==0 || n->get_file_info()->get_isPartOfTransformation());

     ROSE_ASSERT(n != NULL);
     ROSE_ASSERT(n->get_file_info() != NULL);

  // DQ (5/10/2005): For now let's check both, but I think we want to favor isTransfrmation() over get_isPartOfTransformation() in the future.
     return (n->get_file_info()->isTransformation() || n->get_file_info()->get_isPartOfTransformation());
   }

bool
Unparser::isCompilerGenerated( SgLocatedNode *n)
   {
     ROSE_ASSERT(n != NULL);
     ROSE_ASSERT(n->get_file_info() != NULL);

  // DQ (5/22/2005): Support for including any compiler generated code (such as instatiated templates).
     return n->get_file_info()->isCompilerGenerated();
   }

bool
Unparser::containsLanguageStatements ( char* fileName )
   {
  // We need to implement this later
     ROSE_ASSERT (fileName != NULL);
  // printf ("Warning: Unparser::containsLanguageStatements(%s) not implemented! \n",fileName);

  // Note that: false will imply that the file contains only preprocessor declarations
  //                  and thus the file need not be unparsed with a different name
  //                  (a local header file).
  //            true  will imply that it needs to be unparsed as a special renamed 
  //                  header file so that transformations in the header files can be
  //                  supported.

#if 1
     return false;
#else
     return true;
#endif
   }

bool
Unparser::includeFileIsSurroundedByExternCBraces ( char* tempFilename )
   {
  // For the first attempt at writing this function we will check to see if all the
  // declarations in the target file have "C" linkage.  This is neccessary but not 
  // a sufficent condition to knowing if the user really specific a surrounding 
  // extern "C" around an include file.

  // A more robust test is to search all the declaration until the next declaration coming
  // from the current file (if they ALL have "C" linkage then it is at least equivalent to
  // unparsing with a surrounding extern "C" (with braces) specification.

  // For the moment just assume that all files require or don't require extern "C" linkage.
  // Why is this not good enough for compiling (if we don't link anything)?

#if 1
     return false;
#else
     return true;
#endif
   }

//-----------------------------------------------------------------------------------
//  int Unparser::line_count
//
//  counts the number of lines in one directive
//-----------------------------------------------------------------------------------
int Unparser::line_count(char* directive) {
  int lines = 1;
  
  for (int i = 0; directive[i] != '\0'; i++) {
    if (directive[i] == '\n')
      lines++;
  }
  return lines;
}


bool
Unparser::isASecondaryFile ( SgStatement* stmt )
   {
  // for now just assume there are no secondary files
     return false;
   }

#if 0
// DQ (10/11/2007): This does not appear to be used any more (member functions in the SgProject 
// call the member functions in the SgFile which call the  unparseFile function 
// directly).
void
Unparser::unparseProject ( SgProject* project, SgUnparse_Info& info )
   {
     ROSE_ASSERT(project != NULL);
     SgFile* file = &(project->get_file(0));
     ROSE_ASSERT(file != NULL);

     unparseFile(file, info);
   }
#endif

// DQ (9/2/2008): Seperate out the details of unparsing source files from binary files.
void
Unparser::unparseFile ( SgSourceFile* file, SgUnparse_Info& info )
   {
  // ROSE_ASSERT(file != NULL);
  // unparseFile (file,info);

     ROSE_ASSERT(file != NULL);

  // Detect reuse of an Unparser object with a different file
     ROSE_ASSERT(currentFile == NULL);

     currentFile = file;
     ROSE_ASSERT(currentFile != NULL);

  // file->display("file: Unparser::unparseFile");

#if 1
  // DQ (11/10/2007): Moved computation of hidden list from astPostProcessing.C to unparseFile so that 
  // it will be called AFTER any transformations and immediately before code generation where it is 
  // really required.  This part of a fix for Liao's outliner, but should be useful for numerous 
  // transformations.  This also make simple analysis much cheaper since the hidel list computation is
  // expensive (in this implementation).
  // DQ (8/6/2007): Only compute the hidden lists if working with C++ code!
     if (SageInterface::is_Cxx_language() == true)
        {
       // DQ (5/22/2007): Moved from SgProject::parse() function to here so that propagateHiddenListData() could be called afterward.
       // DQ (5/8/2007): Now build the hidden lists for types and declarations (Robert Preissl's work)
          Hidden_List_Computation::buildHiddenTypeAndDeclarationLists(file);

       // DQ (6/5/2007): We actually need this now since the hidden lists are not pushed to lower scopes where they are required.
       // DQ (5/22/2007): Added support for passing hidden list information about types, declarations and elaborated types to child scopes.
          propagateHiddenListData(file);
        }
#endif

  // Turn ON the error checking which triggers an if the default SgUnparse_Info constructor is called
     SgUnparse_Info::set_forceDefaultConstructorToTriggerError(true);

     if (file->get_markGeneratedFiles() == true)
        {
       // Output marker to identify code generated by ROSE (causes "#define ROSE_GENERATED_CODE" to be placed at the top of the file).
       // printf ("Output marker to identify code generated by ROSE \n");
          u_exprStmt->markGeneratedFile();
        }

  // SgScopeStatement* globalScope = (SgScopeStatement*) (&(file->root()));
     SgScopeStatement* globalScope = file->get_globalScope();
     ROSE_ASSERT(globalScope != NULL);

  // Make sure that both the C/C++ and Fortran unparsers are present!
     ROSE_ASSERT(u_exprStmt != NULL);
     ROSE_ASSERT(u_fortran_locatedNode != NULL);

     ROSE_ASSERT(file->get_outputLanguage() != SgFile::e_error_output_language);
     ROSE_ASSERT(file->get_outputLanguage() != SgFile::e_Promela_output_language);

  // Use the information in the SgFile object to control which unparser is called.
     if ( ( (file->get_Fortran_only() == true) && (file->get_outputLanguage() == SgFile::e_default_output_language) ) || (file->get_outputLanguage() == SgFile::e_Fortran_output_language) )
        {
       // Unparse using the new Fortran unparser!
          u_fortran_locatedNode->unparseStatement(globalScope, info);
        }
       else
        {
          if ( ( ( (file->get_C_only() == true) || (file->get_Cxx_only() == true) ) && (file->get_outputLanguage() == SgFile::e_default_output_language) ) || 
               ( (file->get_outputLanguage() == SgFile::e_C_output_language) || (file->get_outputLanguage() == SgFile::e_Cxx_output_language) ) )
             {
            // Unparse using C/C++ unparser by default
                u_exprStmt->unparseStatement(globalScope, info);
             }
            else
             {
               if (file->get_PHP_only())
                  {
                    Unparse_PHP unparser(this,file->get_unparse_output_filename());
                    unparser.unparseStatement(globalScope, info);
                  }
                 else
                  {
                    printf ("Error: unclear how to unparse the input code! \n");
                    ROSE_ASSERT(false);
                  }
             }
        }

  // DQ (7/19/2004): Added newline at end of file
  // (some compilers (e.g. g++) complain if no newline is present)
  // This does not work, not sure why
  // cur << "\n/* EOF: can't insert newline at end of file to avoid g++ compiler warning */ \n\n";

  // DQ: This does not compile
  // cur << std::endl;

  // DQ: This does not force a new line either!
  // cur << "\n\n\n";
     cur.flush();

  // Turn OFF the error checking which triggers an if the default SgUnparse_Info constructor is called
     SgUnparse_Info::set_forceDefaultConstructorToTriggerError(false);
   }

// DQ (9/2/2008): Seperate out the details of unparsing source files from binary files.
void
Unparser::unparseAsmFile ( SgAsmFile* asmFile, SgUnparse_Info& info )
   {
  // This is code that is refactored so that it can be called form a much shorter 
  // Unparser::unparseFile() function (below).

     ROSE_ASSERT(asmFile != NULL);

  // ROSE_ASSERT(asmFile->get_binary_only() == true) ;
  // file->display("file: binary unparse");

  // string outputFileName = "unparse.s";
  // string outputFileName = asmFile->get_unparse_output_filename();
     string outputFileName = asmFile->get_name();

  // DQ (8/14/2008): Added test to make sure that there is a valid SgAsmBlock with instructions.
  // So that we can optionally just test the binary file format details.
     unparseAsmFileToFile(outputFileName, asmFile);

  // DQ (8/20/2008): Output the re-assembled binary from the parts in the represnetation of the binary file format 
  // (Note that this does not support transformations on instructions, so this is not a backend code generator).

  // Writes a new executable based on the parse tree. The new executable should be byte-for-byte identical with 
  // the original.  This work supports testing that we have completely represented the binary executable file format.
  // Any transformations to parts of the binary executable file format (in the AST) will be represented in the 
  // re-generated binary. Note that we didn't have to disassemble the instructions to re-generate the binary, that
  // is an orthogonal concept (so this step does not depend upon the value of get_read_executable_file_format_only()).
#if 0
  // The output filename should perhaps be the one in: file->get_unparse_output_filename()
  // Need to think about this, since it might overwrite the input binary executable.
     string newFilename = file->get_unparse_output_filename();
#else
  // string sourceFilename = asmFile->get_sourceFileNameWithoutPath();
     string sourceFilename = asmFile->get_name();

     printf ("In Unparser::unparseAsmFile(): sourceFilename = %s \n",sourceFilename.c_str());

  // DQ (8/30/2008): This is temporary, we should review how we want to name the files 
  // generated in the unparse phase of processing a binary.
     string newFilename = sourceFilename + ".new";
     size_t slash = sourceFilename.find_last_of('/');
     if (slash!=sourceFilename.npos)
          newFilename.replace(0, slash+1, "");

  // DQ (8/21/2008): Only output a message when we we use verbose option.
     if ( SgProject::get_verbose() >= 1 )
          std::cout << "output re-generated binary as: " << newFilename << std::endl;
#endif

#if 0
  // Regenerate the binary executable.
     SgAsmExecutableFileFormat::unparseBinaryFormat(newFilename, asmFile);
#else
     printf ("WARNING: SKIPPING SgAsmExecutableFileFormat::unparseBinaryFormat() \n");
#endif

  // Dump detailed info from the AST representation of the binary executable file format.
  // string baseName = file->get_sourceFileNameWithoutPath();
     string baseName = asmFile->get_name();

  // DQ (8/30/2008): This is temporary, we should review how we want to name the files 
  // generated in the unparse phase of processing a binary.
     std::string dumpName = baseName + ".dump";

     printf ("In Unparser::unparseAsmFile(): dumpName = %s \n",dumpName.c_str());

     FILE *dumpFile = fopen(dumpName.c_str(), "wb");

  // DQ (2/3/2009): Added assertion.
     ROSE_ASSERT(dumpFile != NULL);

     if (dumpFile != NULL)
        {
          SgAsmGenericFile *genericFile = asmFile->get_genericFile();
          ROSE_ASSERT(genericFile != NULL);

       // The file type should be the first; test harness depends on it
          fprintf(dumpFile, "%s\n", genericFile->format_name());

       // A table describing the sections of the file
          genericFile->dump(dumpFile);

       // Detailed info about each section
          const SgAsmGenericSectionPtrList &sections = genericFile->get_sections();
          for (size_t i = 0; i < sections.size(); i++)
             {
            // printf ("In unparser: output section #%zu \n",i);
               fprintf(dumpFile, "Section [%zd]:\n", i);

               ROSE_ASSERT(sections[i] != NULL);
#if 1
               sections[i]->dump(dumpFile, "  ", -1);
#else
               printf ("WARNING: SKIPPING SgAsmGenericSection::dump() for i = %zu \n",i);
#endif
               bool outputInstruction = (sections[i]->is_mapped() == true);

#if 0
            // Handle special case of Extended DOS Header (initial part of PE files)
               if (sections[i]->get_mapped() == false && sections[i]->get_name() == "Extended DOS Header")
                  {
                    printf ("Handling the special case of the Extended DOS Header \n");
                    outputInstruction = true;
                  }
#endif

            // If this section was mapped to memory then output the associated instructions 
            // that have been disassembled.
            // printf ("Section [%zd]: outputInstruction = %s \n",i,outputInstruction ? "true" : "false"); 
               if (outputInstruction == true)
                  {
                 // Output the instructions
                    SgAsmGenericHeader* genericHeader = sections[i]->get_header();
                    ROSE_ASSERT(genericHeader != NULL);

                 // printf ("header name = %s \n",genericHeader->get_name()->c_str());

                    rose_addr_t imageBase = genericHeader->get_base_va();
 
                 // printf ("section %s imageBase = 0x%08"PRIx64"\n",sections[i]->get_name()->c_str(),imageBase);

                    rose_addr_t addressBase  =  imageBase + sections[i]->get_mapped_rva();
                    rose_addr_t addressBound =  addressBase + sections[i]->get_mapped_size();

                 // This is an error to uncomment, but used to provide useful information for debugging.
                 // fprintf(f, "%s%-*s = rva=0x%08"PRIx64", size=%"PRIu64" bytes\n", p, w, "mapped",  p_mapped_rva, p_mapped_size);

                 // printf ("section %s starting address = 0x%08"PRIx64" ending address = 0x%08"PRIx64"\n",sections[i]->get_name()->c_str(),addressBase,addressBound);

                 // DQ (9/1/2008): This is part of code to include the disassembled
                 // instructions in a specific range relevant to a section (unfinished).
                 // It is supposed to map around (exclude) address ranges represented 
                 // by merged sections in PE formated binaries.

                 // Build a bitvector of the current section's mapped address space.
                    vector<bool> sectionAddressSpace(sections[i]->get_mapped_size(),true);
                    for (size_t j = 0; j < sections.size(); j++)
                       {
                      // exclude all the other sections
                         if ( (j != i) && (sections[j]->is_mapped() == true) )
                            {
                              SgAsmGenericHeader* genericHeader  = sections[j]->get_header();
                              rose_addr_t temp_imageBase = genericHeader->get_base_va();
                              rose_addr_t nestedAddressBase  = temp_imageBase    + sections[j]->get_mapped_rva();
                              rose_addr_t nestedAddressBound = nestedAddressBase + sections[j]->get_mapped_size();
                           // printf ("Exclude range in section %s starting address = 0x%08"PRIx64" ending address = 0x%08"PRIx64"\n",sections[j]->get_name().c_str(),nestedAddressBase,nestedAddressBound);

#if 0
                              for (rose_addr_t k = nestedAddressBase; k < nestedAddressBound; k++)
                                 {
                                   rose_addr_t relativeNestedAddressBase = nestedAddressBase - addressBase;
                                   if (relativeNestedAddressBase >= 0)
                                        sectionAddressSpace[k] = false;
                                 }
#endif
                            }
                                         
                       }
#if 0
                 // DQ (10/18/2008): I would like to unparse the instructions associated with 
                 // the current section, but I am not sure how best to do that.
                    fprintf(dumpFile, "\n\n");
                    fprintf(dumpFile, "**************************************************\n");
                    fprintf(dumpFile, "Output the disassembled instructions (by section):\n");
                    fprintf(dumpFile, "**************************************************\n");
                    fprintf(dumpFile, "\n");
                    const SgAsmInterpretationPtrList & interps = asmFile->get_interpretations();
                    printf ("interps.size() = %zu \n",interps.size());
                    for (size_t i = 0; i < interps.size(); ++i)
                       {
                      // fprintf(dumpFile, "%s\n", unparseAsmInterpretation(interps[i]).c_str());
                       }
#endif
                  }
             }

       // The instructions are mostly from the ".text" section, but some come from other 
       // sections and it would be useful to distinguish this detail in the dump output.
          fprintf(dumpFile, "\n\n");
          fprintf(dumpFile, "*************************************\n");
          fprintf(dumpFile, "Output the disassembled instructions:\n");
          fprintf(dumpFile, "*************************************\n");
          fprintf(dumpFile, "\n");
          const SgAsmInterpretationPtrList & interps = asmFile->get_interpretations();
          for (size_t i = 0; i < interps.size(); ++i)
             {
               fprintf(dumpFile, "%s\n", unparseAsmInterpretation(interps[i]).c_str());
             }

          fclose(dumpFile);
        }
   }

void
Unparser::unparseFile ( SgBinaryFile* file, SgUnparse_Info& info )
   {
     ROSE_ASSERT(file != NULL);

     ROSE_ASSERT(file->get_binary_only() == true) ;

  // file->display("file: binary unparse");

  // string outputFileName = "unparse.s";
     string outputFileName = file->get_unparse_output_filename();

  // DQ (2/3/2009): We now store a list of SgAsmFile at the SgBinaryFile to support library archives (that can have many object files).
     ROSE_ASSERT(file->get_binaryFileList().empty() == false);
  // SgAsmFile* asmFile = file->get_binaryFile();

#if 0
  // Case #1 this is the code that I want to use!
  // This would be the ideal code to call, and for a non-library archive file there would be just one entry in the list.
  // But the refactored code fails for the case single SgAsmFile (or rather I had to comment out parts so it would not fail).

     for (size_t i = 0; i < file->get_binaryFileList().size(); i++)
        {
          SgAsmFile* asmFile = file->get_binaryFileList()[i];
          ROSE_ASSERT(asmFile != NULL);
          unparseAsmFile(asmFile,info);
        }
#else
  // Case #2 this is at least partiallly refactored code, second choice code that we could use!
  // This is code which at least separates out the two cases of a file that either is or is not a libary archive.
  // This code allows us to call the original code for the case where we have a non-libary archive file (normal executable).
     if (file->get_isLibraryArchive() == true)
        {
          ROSE_ASSERT(file->get_libraryArchiveObjectFileNameList().empty() == false);

          ROSE_ASSERT(file->get_libraryArchiveObjectFileNameList().empty() == (file->get_isLibraryArchive() == false));

          for (size_t i = 0; i < file->get_binaryFileList().size(); i++)
             {
               printf ("Unparse binary AST for file->get_libraryArchiveObjectFileNameList()[%zu] = %s \n",i,file->get_libraryArchiveObjectFileNameList()[i].c_str());

               SgAsmFile* asmFile = file->get_binaryFileList()[i];
               ROSE_ASSERT(asmFile != NULL);

               printf ("Unparse: asmFile->get_name() = %s \n",asmFile->get_name().c_str());

               unparseAsmFile(asmFile,info);
             }
        }
       else
        {
       // This is the case of a normal binary file (non-libary archive file).

       // Start of case where: get_isLibraryArchive() == false
          ROSE_ASSERT(file->get_libraryArchiveObjectFileNameList().empty() == true);

       // string executableFileName = this->get_sourceFileNameWithPath();
#if 0
       // DQ (2/3/2009): This fails (it is the new refactored code)...
          buildAsmAST(this->get_sourceFileNameWithPath());
#else
  // DQ (2/3/2009): Ths is the original code (refactored code fails to execute properly).
  // This is the older code which I tried to refactor, but this work and the refactored code fails.
  // so for the case of a single SgAsmFile in a SgBinaryFile, execut this code until I can figure out
  // what is wrong with the refactored code.

     SgAsmFile* asmFile = file->get_binaryFile();

  // DQ (8/14/2008): Added test to make sure that there is a valid SgAsmBlock with instructions.
  // So that we can optionally just test the binary file format details.
     unparseAsmFileToFile(outputFileName, asmFile);

  // DQ (8/20/2008): Output the re-assembled binary from the parts in the represnetation of the binary file format 
  // (Note that this does not support transformations on instructions, so this is not a backend code generator).

  // Writes a new executable based on the parse tree. The new executable should be byte-for-byte identical with 
  // the original.  This work supports testing that we have completely represented the binary executable file format.
  // Any transformations to parts of the binary executable file format (in the AST) will be represented in the 
  // re-generated binary. Note that we didn't have to disassemble the instructions to re-generate the binary, that
  // is an orthogonal concept (so this step does not depend upon the value of get_read_executable_file_format_only()).
#if 0
  // The output filename should perhaps be the one in: file->get_unparse_output_filename()
  // Need to think about this, since it might overwrite the input binary executable.
     string newFilename = file->get_unparse_output_filename();
#else
     string sourceFilename = file->get_sourceFileNameWithoutPath();

  // DQ (8/30/2008): This is temporary, we should review how we want to name the files 
  // generated in the unparse phase of processing a binary.
     string newFilename = sourceFilename + ".new";
     size_t slash = sourceFilename.find_last_of('/');
     if (slash!=sourceFilename.npos)
          newFilename.replace(0, slash+1, "");

  // DQ (8/21/2008): Only output a message when we we use verbose option.
     if ( SgProject::get_verbose() >= 1 )
          std::cout << "output re-generated binary as: " << newFilename << std::endl;
#endif
  // Regenerate the binary executable.
     SgAsmExecutableFileFormat::unparseBinaryFormat(newFilename, asmFile);

  // Dump detailed info from the AST representation of the binary executable file format.
     string baseName = file->get_sourceFileNameWithoutPath();

  // DQ (8/30/2008): This is temporary, we should review how we want to name the files 
  // generated in the unparse phase of processing a binary.
     std::string dumpName = baseName + ".dump";
     FILE *dumpFile = fopen(dumpName.c_str(), "wb");
     if (dumpFile != NULL)
        {
          SgAsmGenericFile *genericFile = asmFile->get_genericFile();
          ROSE_ASSERT(genericFile != NULL);

       // The file type should be the first; test harness depends on it
          fprintf(dumpFile, "%s\n", genericFile->format_name());

       // A table describing the sections of the file
          genericFile->dump(dumpFile);

       // Detailed info about each section
          const SgAsmGenericSectionPtrList &sections = genericFile->get_sections();
          for (size_t i = 0; i < sections.size(); i++)
             {
            // printf ("In unparser: output section #%zu \n",i);
               fprintf(dumpFile, "Section [%zd]:\n", i);
               sections[i]->dump(dumpFile, "  ", -1);

               bool outputInstruction = (sections[i]->is_mapped() == true);

#if 0
            // Handle special case of Extended DOS Header (initial part of PE files)
               if (sections[i]->get_mapped() == false && sections[i]->get_name() == "Extended DOS Header")
                  {
                    printf ("Handling the special case of the Extended DOS Header \n");
                    outputInstruction = true;
                  }
#endif

            // If this section was mapped to memory then output the associated instructions 
            // that have been disassembled.
            // printf ("Section [%zd]: outputInstruction = %s \n",i,outputInstruction ? "true" : "false"); 
               if (outputInstruction == true)
                  {
                 // Output the instructions
                    SgAsmGenericHeader* genericHeader = sections[i]->get_header();
                    ROSE_ASSERT(genericHeader != NULL);

                 // printf ("header name = %s \n",genericHeader->get_name()->c_str());

                    rose_addr_t imageBase = genericHeader->get_base_va();
 
                 // printf ("section %s imageBase = 0x%08"PRIx64"\n",sections[i]->get_name()->c_str(),imageBase);

                    rose_addr_t addressBase  =  imageBase + sections[i]->get_mapped_rva();
                    rose_addr_t addressBound =  addressBase + sections[i]->get_mapped_size();

                 // This is an error to uncomment, but used to provide useful information for debugging.
                 // fprintf(f, "%s%-*s = rva=0x%08"PRIx64", size=%"PRIu64" bytes\n", p, w, "mapped",  p_mapped_rva, p_mapped_size);

                 // printf ("section %s starting address = 0x%08"PRIx64" ending address = 0x%08"PRIx64"\n",sections[i]->get_name()->c_str(),addressBase,addressBound);

                 // DQ (9/1/2008): This is part of code to include the disassembled
                 // instructions in a specific range relevant to a section (unfinished).
                 // It is supposed to map around (exclude) address ranges represented 
                 // by merged sections in PE formated binaries.

                 // Build a bitvector of the current section's mapped address space.
                    vector<bool> sectionAddressSpace(sections[i]->get_mapped_size(),true);
                    for (size_t j = 0; j < sections.size(); j++)
                       {
                      // exclude all the other sections
                         if ( (j != i) && (sections[j]->is_mapped() == true) )
                            {
                              SgAsmGenericHeader* genericHeader  = sections[j]->get_header();
                              rose_addr_t temp_imageBase = genericHeader->get_base_va();
                              rose_addr_t nestedAddressBase  = temp_imageBase    + sections[j]->get_mapped_rva();
                              rose_addr_t nestedAddressBound = nestedAddressBase + sections[j]->get_mapped_size();
                           // printf ("Exclude range in section %s starting address = 0x%08"PRIx64" ending address = 0x%08"PRIx64"\n",sections[j]->get_name().c_str(),nestedAddressBase,nestedAddressBound);

#if 0
                              for (rose_addr_t k = nestedAddressBase; k < nestedAddressBound; k++)
                                 {
                                   rose_addr_t relativeNestedAddressBase = nestedAddressBase - addressBase;
                                   if (relativeNestedAddressBase >= 0)
                                        sectionAddressSpace[k] = false;
                                 }
#endif
                            }
                                         
                       }
#if 0
                 // DQ (10/18/2008): I would like to unparse the instructions associated with 
                 // the current section, but I am not sure how best to do that.
                    fprintf(dumpFile, "\n\n");
                    fprintf(dumpFile, "**************************************************\n");
                    fprintf(dumpFile, "Output the disassembled instructions (by section):\n");
                    fprintf(dumpFile, "**************************************************\n");
                    fprintf(dumpFile, "\n");
                    const SgAsmInterpretationPtrList & interps = asmFile->get_interpretations();
                    printf ("interps.size() = %zu \n",interps.size());
                    for (size_t i = 0; i < interps.size(); ++i)
                       {
                      // fprintf(dumpFile, "%s\n", unparseAsmInterpretation(interps[i]).c_str());
                       }
#endif
                  }
             }

       // The instructions are mostly from the ".text" section, but some come from other 
       // sections and it would be useful to distinguish this detail in the dump output.
          fprintf(dumpFile, "\n\n");
          fprintf(dumpFile, "*************************************\n");
          fprintf(dumpFile, "Output the disassembled instructions:\n");
          fprintf(dumpFile, "*************************************\n");
          fprintf(dumpFile, "\n");
          const SgAsmInterpretationPtrList & interps = asmFile->get_interpretations();
          for (size_t i = 0; i < interps.size(); ++i)
             {
               fprintf(dumpFile, "%s\n", unparseAsmInterpretation(interps[i]).c_str());
             }

          fclose(dumpFile);
        }

// End of case where: get_isLibraryArchive() == false
        }
#endif
#endif
   }

#if 0
void
Unparser::unparseFile ( SgFile* file, SgUnparse_Info& info )
   {
     ROSE_ASSERT(file != NULL);

  // Detect reuse of an Unparser object with a different file
     ROSE_ASSERT(currentFile == NULL);

     currentFile = file;
     ROSE_ASSERT(currentFile != NULL);

  // file->display("file: Unparser::unparseFile");

#if 1
  // DQ (11/10/2007): Moved computation of hidden list from astPostProcessing.C to unparseFile so that 
  // it will be called AFTER any transformations and immediately before code generation where it is 
  // really required.  This part of a fix for Liao's outliner, but should be useful for numerous 
  // transformations.  This also make simple analysis much cheaper since the hidel list computation is
  // expensive (in this implementation).
  // DQ (8/6/2007): Only compute the hidden lists if working with C++ code!
     if (SageInterface::is_Cxx_language() == true)
        {
       // DQ (5/22/2007): Moved from SgProject::parse() function to here so that propagateHiddenListData() could be called afterward.
       // DQ (5/8/2007): Now build the hidden lists for types and declarations (Robert Preissl's work)
          Hidden_List_Computation::buildHiddenTypeAndDeclarationLists(file);

       // DQ (6/5/2007): We actually need this now since the hidden lists are not pushed to lower scopes where they are required.
       // DQ (5/22/2007): Added support for passing hidden list information about types, declarations and elaborated types to child scopes.
          propagateHiddenListData(file);
        }
#endif

  // Turn ON the error checking which triggers an if the default SgUnparse_Info constructor is called
     SgUnparse_Info::set_forceDefaultConstructorToTriggerError(true);

     if (file->get_markGeneratedFiles() == true)
        {
       // Output marker to identify code generated by ROSE (causes "#define ROSE_GENERATED_CODE" to be placed at the top of the file).
       // printf ("Output marker to identify code generated by ROSE \n");
          u_exprStmt->markGeneratedFile();
        }

     SgScopeStatement* globalScope = (SgScopeStatement*) (&(file->root()));
     ROSE_ASSERT(globalScope != NULL);

  // Make sure that both the C/C++ and Fortran unparsers are present!
     ROSE_ASSERT(u_exprStmt != NULL);
     ROSE_ASSERT(u_fortran_locatedNode != NULL);

     ROSE_ASSERT(file->get_outputLanguage() != SgFile::e_error_output_language);
     ROSE_ASSERT(file->get_outputLanguage() != SgFile::e_Promela_output_language);

  // Use the information in the SgFile object to control which unparser is called.
     if ( ( (file->get_Fortran_only() == true) && (file->get_outputLanguage() == SgFile::e_default_output_language) ) || (file->get_outputLanguage() == SgFile::e_Fortran_output_language) )
        {
       // Unparse using the new Fortran unparser!
          u_fortran_locatedNode->unparseStatement(globalScope, info);
        }
       else
        {
          if ( ( ( (file->get_C_only() == true) || (file->get_Cxx_only() == true) ) && (file->get_outputLanguage() == SgFile::e_default_output_language) ) || 
               ( (file->get_outputLanguage() == SgFile::e_C_output_language) || (file->get_outputLanguage() == SgFile::e_Cxx_output_language) ) )
             {
            // Unparse using C/C++ unparser by default
                u_exprStmt->unparseStatement(globalScope, info);
             }
            else
             {
               if (file->get_PHP_only())
                  {
                    Unparse_PHP unparser(this,file->get_unparse_output_filename());
                    unparser.unparseStatement(globalScope, info);
                  }
                 else
                  {
                    printf ("Error: unclear how to unparse the input code! \n");        
                    ROSE_ASSERT(false);
                  }
             }
        }

  // DQ (7/19/2004): Added newline at end of file
  // (some compilers (e.g. g++) complain if no newline is present)
  // This does not work, not sure why
  // cur << "\n/* EOF: can't insert newline at end of file to avoid g++ compiler warning */ \n\n";

  // DQ: This does not compile
  // cur << std::endl;

  // DQ: This does not force a new line either!
  // cur << "\n\n\n";
     cur.flush();

  // Turn OFF the error checking which triggers an if the default SgUnparse_Info constructor is called
     SgUnparse_Info::set_forceDefaultConstructorToTriggerError(false);
   }
#endif


string
unparseStatementWithoutBasicBlockToString ( SgStatement* statement )
   {
      string statementString;

      ROSE_ASSERT ( statement != NULL );
   // printf ("unparseStatementWithoutBasicBlockToString(): statement->sage_class_name() = %s \n",statement->sage_class_name());

   // Build a SgUnparse_Info object to represent formatting options for
   // this statement (use the default values).
      SgUnparse_Info info;

   // exclude comments
      info.set_SkipComments();

   // exclude body and the trailing semicolon
      info.set_SkipBasicBlock();
      info.set_SkipSemiColon();

   // exclude all CPP directives (since they have already been evaluated by the front-end)
      info.set_SkipCPPDirectives();

      switch ( statement->variantT() )
        {
          case V_SgCaseOptionStmt:
          case V_SgDefaultOptionStmt:
               statementString = globalUnparseToString(statement,&info);
          break;

          default:
               printf ("Error, default case in switch within unparseStatementWithoutBasicBlockToString() \n");
        }

     printf ("In unparseStatementWithoutBasicBlockToString(): statementString = %s \n",statementString.c_str());

     return statementString;
   }

string
unparseScopeStatementWithoutBasicBlockToString ( SgScopeStatement* scope )
   {
      string scopeString;

      ROSE_ASSERT ( scope != NULL );
   // printf ("unparseScopeStatementWithoutBasicBlockToString(): scope->sage_class_name() = %s \n",scope->sage_class_name());

   // Build a SgUnparse_Info object to represent formatting options for
   // this statement (use the default values).
      SgUnparse_Info info;

   // exclude comments
      info.set_SkipComments();

   // exclude body and the trailing semicolon
      info.set_SkipBasicBlock();
      info.set_SkipSemiColon();

   // exclude all CPP directives (since they have already been evaluated by the front-end)
      info.set_SkipCPPDirectives();

      switch ( scope->variantT() )
        {
          case V_SgSwitchStatement:
          case V_SgForStatement:
          case V_SgWhileStmt:
          case V_SgDoWhileStmt:
          case V_SgIfStmt:
          case V_SgCatchOptionStmt:
          case V_SgCaseOptionStmt:
               scopeString = globalUnparseToString(scope,&info);
          break;
          default:
               printf ("Error, default case in switch within unparseScopeStatementWithoutBasicBlockToString() \n");
               ROSE_ASSERT (false);
        }

  // printf ("In unparseScopeStatementWithoutBasicBlockToString(): scopeString = %s \n",scopeString.c_str());

     return scopeString;
   }

string
unparseDeclarationToString ( SgDeclarationStatement* declaration, bool unparseAsDeclaration )
   {
  // This function generates a string for a declaration. The string is required for 
  // the intermediate file to make sure that all transformation code will compile 
  // (since it could depend on declarations defined within the code).

  // Details:
  //   1) Only record declarations found within the source file (exclude all header files 
  //      since they will be seen when the same header files are included).
  //   2) Resort the variable declarations to remove redundent entries.
  //        WRONG: variable declarations could have dependences upon class declarations!
  //   3) Don't sort all declarations since some could have dependences.
  //        a) class declarations
  //        b) typedefs
  //        c) function declarations
  //        d) template declarations
  //        e) variable definition???

  // ROSE_ASSERT (this != NULL);
     ROSE_ASSERT ( declaration != NULL );

#if 0
     printf ("generateDeclarationToString(): unparseAsDeclaration = %s declaration->sage_class_name() = %s \n",
     unparseAsDeclaration ? "true" : "false",declaration->sage_class_name());
#endif

     string declarationString;

  // Build a SgUnparse_Info object to represent formatting options for
  // this statement (use the default values).
     SgUnparse_Info info;

   // exclude comments
      info.set_SkipComments();

   // exclude all CPP directives (since they have already been evaluated by the front-end)
      info.set_SkipCPPDirectives();

      switch ( declaration->variantT() )
        {
       // Enum declarations should not skip their definition since 
       // this is where the constants are declared.
          case V_SgEnumDeclaration:

          case V_SgVariableDeclaration:
          case V_SgTemplateDeclaration:
          case V_SgTypedefDeclaration:
            // Need to figure out if a forward declaration would work or be 
            // more conservative and always output the complete class definition.

            // turn off output of initializer values
               info.set_SkipInitializer();
            // output the declaration as a string
               declarationString = globalUnparseToString(declaration,&info);
               break;

          case V_SgClassDeclaration:
            // Need to figure out if a forward declaration would work or be 
            // more conservative and always output the complete class definition.

            // turn off the generation of the function definitions only 
            // (we still want the restof the class definition since these 
            // define all member data and member functions).
               info.set_SkipFunctionDefinition();
               info.set_AddSemiColonAfterDeclaration();

               if (unparseAsDeclaration == false)
                  {
                    info.set_SkipClassDefinition();
                  }
                 else
                  {
//                  info.set_AddSemiColonAfterDeclaration();
                  }

            // output the declaration as a string
               declarationString = globalUnparseToString(declaration,&info);
               break;

       // For functions just output the declaration and skip the definition
       // (This also avoids the generation of redundent definitions since the 
       // function we are in when we generate all declarations would be included).
          case V_SgMemberFunctionDeclaration:
          case V_SgFunctionDeclaration:
             {
            // turn off the generation of the definition
               info.set_SkipFunctionDefinition();

            // unparse with the ";" (as a declaration) or unparse without the ";" 
            // as a function for which we will attach a local scope (basic block).
            // printf ("In generateDeclarationString(): unparseAsDeclaration = %s \n",(unparseAsDeclaration) ? "true" : "false");
               if (unparseAsDeclaration == true)
                    info.set_AddSemiColonAfterDeclaration();

            // output the declaration as a string
               declarationString = globalUnparseToString(declaration,&info);
               break;
             }

          case V_SgFunctionParameterList:
             {
            // Handle generation of declaration strings this case differently from unparser
            // since want to generate declaration strings and not function parameter lists
            // (function parameter lists would be delimited by "," while declarations would
            // be delimited by ";").
               SgFunctionParameterList* parameterListDeclaration = dynamic_cast<SgFunctionParameterList*>(declaration);
               ROSE_ASSERT (parameterListDeclaration != NULL);
               SgInitializedNamePtrList & argList = parameterListDeclaration->get_args();
               SgInitializedNamePtrList::iterator i;
               for (i = argList.begin(); i != argList.end(); i++)
                  {
//                  printf ("START: Calling unparseToString on type! \n");
                    ROSE_ASSERT ((*i) != NULL);
                    string typeNameString = (*i)->get_type()->unparseToString();
//                  printf ("DONE: Calling unparseToString on type! \n");

                    string variableName;
                    if ( (*i)->get_name().getString() != "")
                       {
                         variableName   = (*i)->get_name().str();
                         declarationString += typeNameString + " " + variableName + "; ";
                       }
                      else
                       {
                      // Don't need the tailing ";" if there is no variable name (I think)
                         declarationString += typeNameString + " ";
                       }
                  }
               break;
             }

       // ignore this case ... not really a declaration
          case V_SgCtorInitializerList:
            // printf ("Ignore the SgCtorInitializerList (constructor initializer list) \n");
               break;

          case V_SgVariableDefinition:
               printf ("ERROR: SgVariableDefinition nodes not used in AST \n");
               ROSE_ABORT();
               break;

       // DQ (7/31/2006): Suggested additions by Markus Schordan.
          case V_SgPragma:
               break;
          case V_SgPragmaDeclaration:
               break;

       // default case should always be an error
          default:
               printf ("Default reached in AST_Rewrite::AccumulatedDeclarationsAttribute::generateDeclarationString() \n");
               printf ("     declaration->sage_class_name() = %s \n",declaration->sage_class_name());
               ROSE_ABORT();
               break;
        }

  // Add a space to make it easier to read (not required)
     declarationString += " ";

  // printf ("For this scope: declarationString = %s \n",declarationString.c_str());

     return declarationString;
   }


string
Unparser::removeUnwantedWhiteSpace ( const string & X )
   {
     string returnString;
     int stringLength = X.length();

     for (int i=0; i < stringLength; i++)
        {
          if ( (X[i] != ' ') && (X[i] != '\n') )
             {
               returnString += X[i];
             }
            else
             {
               if ( (i > 0) && (X[i] == ' ') &&
                  ( (X[i-1] != ' ') && (X[i-1] != '\n') && (X[i-1] != '{') && (X[i-1] != ';') && (X[i-1] != '}')) )
                  {
                    if ( (i < stringLength-1) && (X[i] == ' ') && (X[i+1] != '(') )
                         returnString += X[i];
                  }
             }
        }

     return returnString;
   }


// DQ (12/5/2006): Output separate file containing source position information for highlighting (useful for debugging).
int
Unparser::get_embedColorCodesInGeneratedCode()
   {
     return embedColorCodesInGeneratedCode;
   }

int
Unparser::get_generateSourcePositionCodes()
   {
     return generateSourcePositionCodes;
   }

void
Unparser::set_embedColorCodesInGeneratedCode( int x )
   {
     embedColorCodesInGeneratedCode = x;
   }

void
Unparser::set_generateSourcePositionCodes( int x )
   {
     generateSourcePositionCodes = x;
   }


 /*! \brief This function is the connection from the SgNode::unparseToString() function 
            to the unparser.

     This function connects the SgNode::unparseToString() function
     to the unparser.  It takes an optional SgUnparse_Info object pointer.
     If a SgUnparse_Info object is provided then it is not deleted by this
     function.

     \internal Internally this function allocates a SgUnparse_Info object if one is
               not proviede within the function interface.  If the function allocates
               a SgUnparse_Info object it will delete it.  The user is always responcible
               for the allocation and destruction of objects provided to the interface 
               of functions.
  */
string
globalUnparseToString_OpenMPSafe ( const SgNode* astNode, SgUnparse_Info* inputUnparseInfoPointer );

string
globalUnparseToString ( const SgNode* astNode, SgUnparse_Info* inputUnparseInfoPointer )
   {
     string returnString;
// tps (Jun 24 2008) added because OpenMP crashes all the time at the unparser
#pragma omp critical (unparser)
     {
       returnString=globalUnparseToString_OpenMPSafe(astNode,inputUnparseInfoPointer);
     }
     return returnString;
   }

string
globalUnparseToString_OpenMPSafe ( const SgNode* astNode, SgUnparse_Info* inputUnparseInfoPointer )
   {
  // This global function permits any SgNode (including it's subtree) to be turned into a string

  // DQ (3/2/2006): Let's make sure we have a valid IR node!
     ROSE_ASSERT(astNode != NULL);

     string returnString;

  // all options are now defined to be false. When these options can be passed in
  // from the prompt, these options will be set accordingly.
     bool _auto                         = false;
     bool linefile                      = false;
     bool useOverloadedOperators        = false;
     bool num                           = false;

  // It is an error to have this always turned off (e.g. pointer = this; will not unparse correctly)
     bool _this                         = true;

     bool caststring                    = false;
     bool _debug                        = false;
     bool _class                        = false;
     bool _forced_transformation_format = false;
     bool _unparse_includes             = false;

  // printf ("In globalUnparseToString(): astNode->sage_class_name() = %s \n",astNode->sage_class_name());

     Unparser_Opt roseOptions( _auto,
                               linefile,
                               useOverloadedOperators,
                               num,
                               _this,
                               caststring,
                               _debug,
                               _class,
                               _forced_transformation_format,
                               _unparse_includes );

  // DQ (7/19/2007): Remove lineNumber from constructor parameter list.
  // int lineNumber = 0;  // Zero indicates that ALL lines should be unparsed

     // Initialize the Unparser using a special string stream inplace of the usual file stream 
     ostringstream outputString;

     const SgLocatedNode* locatedNode = isSgLocatedNode(astNode);
     string fileNameOfStatementsToUnparse;
     if (locatedNode == NULL)
        {
       // printf ("WARNING: applying AST -> string for non expression/statement AST objects \n");
          fileNameOfStatementsToUnparse = "defaultFileNameInGlobalUnparseToString";
        }
       else
        {
          ROSE_ASSERT (locatedNode != NULL);

       // DQ (5/31/2005): Get the filename from a traversal back through the parents to the SgFile
       // fileNameOfStatementsToUnparse = locatedNode->getFileName();
       // fileNameOfStatementsToUnparse = ROSE::getFileNameByTraversalBackToFileNode(locatedNode);
          if (locatedNode->get_parent() == NULL)
             {
            // DQ (7/29/2005):
            // Allow this function to be called with disconnected AST fragments not connected to 
            // a previously generated AST.  This happens in Qing's interface where AST fragements 
            // are built and meant to be unparsed.  Only the parent of the root of the AST 
            // fragement is expected to be NULL.
            // fileNameOfStatementsToUnparse = locatedNode->getFileName();
               fileNameOfStatementsToUnparse = locatedNode->getFilenameString();
             }
            else
             {
            // DQ (2/20/2007): The expression being unparsed could be one contained in a SgArrayType
               SgArrayType* arrayType = isSgArrayType(locatedNode->get_parent());
               if (arrayType != NULL)
                  {
                 // If this is an index of a SgArrayType node then handle as a special case
                    fileNameOfStatementsToUnparse = "defaultFileNameInGlobalUnparseToString";
                  }
                 else
                  {
#if 1
                    fileNameOfStatementsToUnparse = ROSE::getFileNameByTraversalBackToFileNode(locatedNode);
#else
                    SgSourceFile* sourceFile = TransformationSupport::getSourceFile(locatedNode);
                    ROSE_ASSERT(sourceFile != NULL);
                    fileNameOfStatementsToUnparse = sourceFile->getFileName();
#endif
                  }
             }
        }

     ROSE_ASSERT (fileNameOfStatementsToUnparse.size() > 0);

  // Unparser roseUnparser ( &outputString, fileNameOfStatementsToUnparse, roseOptions, lineNumber );
     Unparser roseUnparser ( &outputString, fileNameOfStatementsToUnparse, roseOptions );

  // Information that is passed down through the tree (inherited attribute)
  // Use the input SgUnparse_Info object if it is available.
     SgUnparse_Info* inheritedAttributeInfoPointer = NULL;
     if (inputUnparseInfoPointer != NULL)
        {
       // printf ("Using the input inputUnparseInfoPointer object \n");

       // Use the user provided SgUnparse_Info object
          inheritedAttributeInfoPointer = inputUnparseInfoPointer;
        }
       else
        {
       // DEFINE DEFAULT BEHAVIOUR FOR THE CASE WHEN NO inputUnparseInfoPointer (== NULL) IS 
       // PASSED AS ARGUMENT TO THE FUNCTION
       // printf ("Building a new Unparse_Info object \n");

       // If no input parameter has been specified then allocate one
       // inheritedAttributeInfoPointer = new SgUnparse_Info (NO_UNPARSE_INFO);
          inheritedAttributeInfoPointer = new SgUnparse_Info();
          ROSE_ASSERT (inheritedAttributeInfoPointer != NULL);

       // MS: 09/30/2003: comments de-activated in unparsing
          ROSE_ASSERT (inheritedAttributeInfoPointer->SkipComments() == false);

       // Skip all comments in unparsing
          inheritedAttributeInfoPointer->set_SkipComments();
          ROSE_ASSERT (inheritedAttributeInfoPointer->SkipComments() == true);
       // Skip all whitespace in unparsing (removed in generated string)
          inheritedAttributeInfoPointer->set_SkipWhitespaces();
          ROSE_ASSERT (inheritedAttributeInfoPointer->SkipWhitespaces() == true);

       // Skip all directives (macros are already substituted by the front-end, so this has no effect on those)
          inheritedAttributeInfoPointer->set_SkipCPPDirectives();
          ROSE_ASSERT (inheritedAttributeInfoPointer->SkipCPPDirectives() == true);

#if 1
       // DQ (8/1/2007): Test if we can force the default to be to unparse fully qualified names.
       // printf ("Setting the default to generate fully qualified names, astNode = %p = %s \n",astNode,astNode->class_name().c_str());
          inheritedAttributeInfoPointer->set_forceQualifiedNames();

       // DQ (8/6/2007): Avoid output of "public", "private", and "protected" in front of class members.
       // This does not appear to have any effect, because it it explicitly set in the unparse function 
       // for SgMemberFunctionDeclaration.
          inheritedAttributeInfoPointer->unset_CheckAccess();

       // DQ (8/1/2007): Only try to set the current scope to the SgGlobal scope if this is NOT a SgProject or SgFile
          if ( (isSgProject(astNode) != NULL || isSgFile(astNode) != NULL ) == false )
             {
            // This will be set to NULL where astNode is a SgType!
               inheritedAttributeInfoPointer->set_current_scope(TransformationSupport::getGlobalScope(astNode));
             }
#endif
        }

     ROSE_ASSERT (inheritedAttributeInfoPointer != NULL);
     SgUnparse_Info & inheritedAttributeInfo = *inheritedAttributeInfoPointer;

  // DQ (5/27/2007): Commented out, uncomment when we are ready for Robert's new hidden list mechanism.
     if (inheritedAttributeInfo.get_current_scope() == NULL)
        {
       // printf ("In globalUnparseToString(): inheritedAttributeInfo.get_current_scope() == NULL astNode = %p = %s \n",astNode,astNode->class_name().c_str());

       // DQ (6/2/2007): Find the nearest containing scope so that we can fill in the current_scope, so that the name qualification can work.
#if 1
          SgStatement* stmt = TransformationSupport::getStatement(astNode);
#else
          SgStatement* stmt = NULL;
       // DQ (6/27/2007): SgProject and SgFile are not contained in any statement
          if (isSgProject(astNode) == NULL && isSgFile(astNode) == NULL)
               stmt = TransformationSupport::getStatement(astNode);
#endif

          if (stmt != NULL)
             {
               SgScopeStatement* scope = stmt->get_scope();
               ROSE_ASSERT(scope != NULL);
               inheritedAttributeInfo.set_current_scope(scope);
             }
            else
             {
            // DQ (6/27/2007): If we unparse a type then we can't find the enclosing statement, so 
            // assume it is SgGlobal. But how do we find a SgGlobal IR node to use?  So we have to 
            // leave it NULL and hand this case downstream!
               inheritedAttributeInfo.set_current_scope(NULL);
             }
#if 1
          const SgTemplateArgument* templateArgument = isSgTemplateArgument(astNode);
          if (templateArgument != NULL)
             {
            // debugging code!
            // printf ("Exiting to debug case of SgTemplateArgument \n");
            // ROSE_ASSERT(false);

               SgScopeStatement* scope = templateArgument->get_scope();
               printf ("SgTemplateArgument case: scope = %p = %s \n",scope,scope->class_name().c_str());
               inheritedAttributeInfo.set_current_scope(scope);
             }
#endif
       // stmt->get_startOfConstruct()->display("In unparseStatement(): info.get_current_scope() == NULL: debug");
       // ROSE_ASSERT(false);
        }
  // ROSE_ASSERT(info.get_current_scope() != NULL);

  // Turn ON the error checking which triggers an error if the default SgUnparse_Info constructor is called
  // SgUnparse_Info::forceDefaultConstructorToTriggerError = true;

#if 1
  // DQ (10/19/2004): Cleaned up this code, remove this dead code after we are sure that this worked properly
  // Actually, this code is required to be this way, since after this branch the current function returns and
  // some data must be cleaned up differently!  So put this back and leave it this way, and remove the
  // "Implementation Note".

  // Both SgProject and SgFile are handled via recursive calls
     if ( (isSgProject(astNode) != NULL) || (isSgSourceFile(astNode) != NULL) )
        {
       // printf ("Implementation Note: Put these cases (unparsing the SgProject and SgFile into the cases for nodes derived from SgSupport below! \n");

       // Handle recursive call for SgProject
          const SgProject* project = isSgProject(astNode);
          if (project != NULL)
             {
               for (int i = 0; i < project->numberOfFiles(); i++)
                  {
                 // SgFile* file = &(project->get_file(i));
                    SgFile* file = project->get_fileList()[i];
                    ROSE_ASSERT(file != NULL);
                    string unparsedFileString = globalUnparseToString_OpenMPSafe(file,inputUnparseInfoPointer);
                 // string prefixString       = string("/* TOP:")      + string(ROSE::getFileName(file)) + string(" */ \n");
                 // string suffixString       = string("\n/* BOTTOM:") + string(ROSE::getFileName(file)) + string(" */ \n\n");
                    string prefixString       = string("/* TOP:")      + file->getFileName() + string(" */ \n");
                    string suffixString       = string("\n/* BOTTOM:") + file->getFileName() + string(" */ \n\n");
                    returnString += prefixString + unparsedFileString + suffixString;
                  }
             }

       // Handle recursive call for SgFile
          const SgSourceFile* file = isSgSourceFile(astNode);
          if (file != NULL)
             {
               SgGlobal* globalScope = file->get_globalScope();
               ROSE_ASSERT(globalScope != NULL);
               returnString = globalUnparseToString_OpenMPSafe(globalScope,inputUnparseInfoPointer);
             }
        }
       else
#endif
        {
       // DQ (1/12/2003): Only now try to trap use of SgUnparse_Info default constructor
       // Turn ON the error checking which triggers an error if the default SgUnparse_Info constructor is called
       // GB (09/27/2007): Took this out because it breaks parallel traversals that call unparseToString. It doesn't
       // seem to have any other effect (whatever was debugged with this seems to be fixed now).
       // SgUnparse_Info::set_forceDefaultConstructorToTriggerError(true);

          if (isSgStatement(astNode) != NULL)
             {
               const SgStatement* stmt = isSgStatement(astNode);

            // DQ (2/2/2007): Note that we should modify the unparser to take the IR nodes as const pointers, but this is a bigger job than I want to do now!
               roseUnparser.u_exprStmt->unparseStatement ( const_cast<SgStatement*>(stmt), inheritedAttributeInfo );
             }

          if (isSgExpression(astNode) != NULL)
             {
               const SgExpression* expr = isSgExpression(astNode);

            // DQ (2/2/2007): Note that we should modify the unparser to take the IR nodes as const pointers, but this is a bigger job than I want to do now!
               roseUnparser.u_exprStmt->unparseExpression ( const_cast<SgExpression*>(expr), inheritedAttributeInfo );
             }

          if (isSgType(astNode) != NULL)
             {
               const SgType* type = isSgType(astNode);

            // DQ (2/2/2007): Note that we should modify the unparser to take the IR nodes as const pointers, but this is a bigger job than I want to do now!
               roseUnparser.u_type->unparseType ( const_cast<SgType*>(type), inheritedAttributeInfo );
             }

          if (isSgSymbol(astNode) != NULL)
             {
               const SgSymbol* symbol = isSgSymbol(astNode);

            // DQ (2/2/2007): Note that we should modify the unparser to take the IR nodes as const pointers, but this is a bigger job than I want to do now!
               roseUnparser.u_sym->unparseSymbol ( const_cast<SgSymbol*>(symbol), inheritedAttributeInfo );
             }

          if (isSgSupport(astNode) != NULL)
             {
            // Handle different specific cases derived from SgSupport 
            // (e.g. template parameters and template arguments).
               switch (astNode->variantT())
                  {
#if 0
                    case V_SgProject:
                       {
                         SgProject* project = isSgProject(astNode);
                         ROSE_ASSERT(project != NULL);
                         for (int i = 0; i < project->numberOfFiles(); i++)
                            {
                              SgFile* file = &(project->get_file(i));
                              ROSE_ASSERT(file != NULL);
                              string unparsedFileString = globalUnparseToString_OpenMPSafe(file,inputUnparseInfoPointer);
                              string prefixString       = string("/* TOP:")      + string(ROSE::getFileName(file)) + string(" */ \n");
                              string suffixString       = string("\n/* BOTTOM:") + string(ROSE::getFileName(file)) + string(" */ \n\n");
                              returnString += prefixString + unparsedFileString + suffixString;
                            }
                         break;
                       }
#error "DEAD CODE!"
                 // case V_SgFile:
                       {
                         SgFile* file = isSgFile(astNode);
                         ROSE_ASSERT(file != NULL);
                         SgGlobal* globalScope = file->get_globalScope();
                         ROSE_ASSERT(globalScope != NULL);
                         returnString = globalUnparseToString_OpenMPSafe(globalScope,inputUnparseInfoPointer);
                         break;
                       }
#endif
                    case V_SgTemplateParameter:
                       {
                         const SgTemplateParameter* templateParameter = isSgTemplateParameter(astNode);

                      // DQ (2/2/2007): Note that we should modify the unparser to take the IR nodes as const pointers, but this is a bigger job than I want to do now!
                         roseUnparser.u_exprStmt->unparseTemplateParameter(const_cast<SgTemplateParameter*>(templateParameter),inheritedAttributeInfo);
                         break;
                       }
                    case V_SgTemplateArgument:
                       {
                         const SgTemplateArgument* templateArgument = isSgTemplateArgument(astNode);

                      // DQ (2/2/2007): Note that we should modify the unparser to take the IR nodes as const pointers, but this is a bigger job than I want to do now!
                         roseUnparser.u_exprStmt->unparseTemplateArgument(const_cast<SgTemplateArgument*>(templateArgument),inheritedAttributeInfo);
                         break;
                       }

                    case V_SgInitializedName:
                       {
                      // DQ (8/6/2007): This should just unparse the name (fully qualified if required).
                      // QY: not sure how to implement this
                      // DQ (7/23/2004): This should unparse as a declaration (type and name with initializer).
                         const SgInitializedName* initializedName = isSgInitializedName(astNode);
                      // roseUnparser.get_output_stream() << initializedName->get_qualified_name().str();
                         SgScopeStatement* scope = initializedName->get_scope();
                         if (isSgGlobal(scope) == NULL && scope->containsOnlyDeclarations() == true)
                              roseUnparser.get_output_stream() << roseUnparser.u_exprStmt->trimGlobalScopeQualifier ( scope->get_qualified_name().getString() ) << "::";
                         roseUnparser.get_output_stream() << initializedName->get_name().str();
                         break;
                       }

                    case V_Sg_File_Info:
                       {
                      // DQ (8/5/2007): This is implemented above as a special case!
                      // DQ (5/11/2006): Not sure how or if we should implement this
                         break;
                       }

                    case V_SgPragma:
                       {
                         const SgPragma* pr = isSgPragma(astNode);
                         SgPragmaDeclaration* decl = isSgPragmaDeclaration(pr->get_parent());
                         ROSE_ASSERT (decl);
                         roseUnparser.u_exprStmt->unparseStatement ( decl, inheritedAttributeInfo );
                         break;
                       }

                 // Perhaps the support for SgFile and SgProject shoud be moved to this location?
                    default:
                         printf ("Error: default reached in node derived from SgSupport astNode = %s \n",astNode->sage_class_name());
                         ROSE_ABORT();
                }
             }

       // Turn OFF the error checking which triggers an if the default SgUnparse_Info constructor is called
       // GB (09/27/2007): Removed this error check, see above.
       // SgUnparse_Info::set_forceDefaultConstructorToTriggerError(false);

       // MS: following is the rewritten code of the above outcommented 
       //     code to support ostringstream instead of ostrstream.
          returnString = outputString.str();

       // Call function to tighten up the code to make it more dense
          if (inheritedAttributeInfo.SkipWhitespaces() == true)
             {
               returnString = roseUnparser.removeUnwantedWhiteSpace ( returnString );
             }

       // delete the allocated SgUnparse_Info object
          if (inputUnparseInfoPointer == NULL)
               delete inheritedAttributeInfoPointer;
        }
     return returnString;
   }

string get_output_filename( SgFile& file)
   {
#if 1
  // DQ (10/15/2005): This can now be made to be a simpler function!
      if (file.get_unparse_output_filename().empty() == true)
         {
           printf ("Error: no output file name specified, use \"-o <output filename>\" option on commandline (see --help for options) \n");
         }
     ROSE_ASSERT(file.get_unparse_output_filename().empty() == false);
     return file.get_unparse_output_filename();
#else
  // Use the filename with ".rose" suffix for the output file from ROSE (the unparsed C++ code file)
  // This allows Makefiles to have suffix rules drive the generation of the *.C.rose from from ROSE
  // and then permits the C++ compiler to be called to generate the *.o file from the *.C.rose file
     char outputFilename[256];

  // if (file.get_unparse_output_filename() != NULL)
     if (file.get_unparse_output_filename().empty() == false)
        {
       // allow the user to specify the name of the output file
#if 0
          printf ("VALID file.get_unparse_output_filename() found: %s \n",
               file.get_unparse_output_filename());
#endif
          sprintf(outputFilename,"%s",file.get_unparse_output_filename());
        }
       else
        {
       // use a prefix plus the original input file name to name the output file
#if 0
          printf ("NO VALID file.get_unparse_output_filename() found, using filename to build one: rose_<%s> \n",
               file.getFileName());
#endif
          sprintf(outputFilename,"rose_%s",file.getFileName());

          printf ("I think this case is never used! Exiting ... \n");
          ROSE_ASSERT (false);
        }
#if 1
     if ( file.get_verbose() == true )
          printf ("\nROSE unparsed outputFile name is %s \n\n",outputFilename);
#endif
     return string(outputFilename);
#endif
   }

#if 1
// DQ (10/11/2007): I think this is redundant with the Unparser::unparseFile() member function
// HOWEVER, this is called by the SgFile::unparse() member function, so it has to be here!

// Later we might want to move this to the SgProject or SgFile support class (generated by ROSETTA)
void
unparseFile ( SgFile* file, UnparseFormatHelp *unparseHelp, UnparseDelegate* unparseDelegate )
   {
  // Call the unparser mechanism

  // printf ("Inside of unparseFile ( SgFile* file ) (using filename = %s) \n",file->get_unparse_output_filename().c_str());

  // debugging assertions
  // ROSE_ASSERT ( file.get_verbose() == true );
  // ROSE_ASSERT ( file.get_skip_unparse() == false );
  // file.set_verbose(true);

     ROSE_ASSERT(file != NULL);

#if 0
  // DQ (5/31/2006): It is a message that I think we can ignore (was a problem for Yarden)
  // DQ (4/21/2006): This would prevent the file from being unparsed twice,
  // but then I am not so sure we want to support that.
     if (file->get_unparse_output_filename().empty() == false)
        {
          printf ("Warning, the unparse_output_filename should be set by the unparser or the backend compilation if not set by the unparser ... \n");
        }
#endif
  // Not that this fails in the AST File I/O tests and since the file in unparsed a second time
  // ROSE_ASSERT (file->get_unparse_output_filename().empty() == true);

  // DQ (4/22/2006): This can be true when the "-E" option is used, but then we should not have called unparse()!
     ROSE_ASSERT(file->get_skip_unparse() == false);

  // If we did unparse an intermediate file then we want to compile that 
  // file instead of the original source file.
     if (file->get_unparse_output_filename().empty() == true)
        {
          string outputFilename = "rose_" + file->get_sourceFileNameWithoutPath();

          if (file->get_binary_only() == true)
             {
            // outputFilename = file->get_sourceFileNameWithoutPath();
               outputFilename += ".s";
             }

          file->set_unparse_output_filename(outputFilename);
          ROSE_ASSERT (file->get_unparse_output_filename().empty() == false);
       // printf ("Inside of SgFile::unparse(UnparseFormatHelp*,UnparseDelegate*) outputFilename = %s \n",outputFilename.c_str());
        }

     if (file->get_skip_unparse() == true)
        {
       // MS: commented out the following output
       // if ( file.get_verbose() == true )
	    // printf ("### ROSE::skip_unparse == true: Skipping all source code generation by ROSE generated preprocessor! \n");
        }
       else
        {
       // Open the file where we will put the generated code
          string outputFilename = get_output_filename(*file);

       // printf ("Calling the unparser: outputFilename = %s \n",outputFilename.c_str());
          fstream ROSE_OutputFile(outputFilename.c_str(),ios::out);
       // ROSE_OutputFile.open(s_file.c_str());

       // DQ (12/8/2007): Added error checking for opening out output file.
          if (!ROSE_OutputFile)
             {
            // throw std::exception("(fstream) error while opening file.");
               printf ("Error detected in opening file %s for output \n",outputFilename.c_str());
               ROSE_ASSERT(false);
             }

       // file.set_unparse_includes(false);
       // ROSE_ASSERT (file.get_unparse_includes() == false);

       // This is the new unparser that Gary Lee is developing
       // The goal of this unparser is to provide formatting
       // similar to that of the original application code
#if 0
          if ( file.get_verbose() == true )
               printf ("Calling the NEWER unparser mechanism: outputFilename = %s \n",outputFilename);
#endif

       // all options are now defined to be false. When these options can be passed in
       // from the prompt, these options will be set accordingly.
          bool UseAutoKeyword                = false;
       // bool linefile                      = false;
          bool generateLineDirectives        = file->get_unparse_line_directives();

       // DQ (6/19/2007): note that test2004_24.C will fail if this is false.
       // If false, this will cause A.operator+(B) to be unparsed as "A+B". This is a confusing point!
          bool useOverloadedOperators        = false;
       // bool useOverloadedOperators        = true;

          bool num                           = false;

       // It is an error to have this always turned off (e.g. pointer = this; will not unparse correctly)
          bool _this                         = true;

          bool caststring                    = false;
          bool _debug                        = false;
          bool _class                        = false;
          bool _forced_transformation_format = false;

       // control unparsing of include files into the source file (default is false)
          bool _unparse_includes             = file->get_unparse_includes();

          Unparser_Opt roseOptions( UseAutoKeyword,
                                    generateLineDirectives,
                                    useOverloadedOperators,
                                    num,
                                    _this,
                                    caststring,
                                    _debug,
                                    _class,
                                    _forced_transformation_format,
                                    _unparse_includes );

       // printf ("ROSE::getFileName(file) = %s \n",ROSE::getFileName(file));
       // printf ("file->get_file_info()->get_filenameString = %s \n",file->get_file_info()->get_filenameString().c_str());

       // DQ (7/19/2007): Remove lineNumber from constructor parameter list.
       // int lineNumber = 0;  // Zero indicates that ALL lines should be unparsed
       // Unparser roseUnparser ( &file, &ROSE_OutputFile, ROSE::getFileName(&file), roseOptions, lineNumber );
       // Unparser roseUnparser ( &ROSE_OutputFile, ROSE::getFileName(&file), roseOptions, lineNumber, NULL, repl );
       // Unparser roseUnparser ( &ROSE_OutputFile, ROSE::getFileName(file), roseOptions, lineNumber, unparseHelp, unparseDelegate );
       // Unparser roseUnparser ( &ROSE_OutputFile, file->get_file_info()->get_filenameString(), roseOptions, lineNumber, unparseHelp, unparseDelegate );

          Unparser roseUnparser ( &ROSE_OutputFile, file->get_file_info()->get_filenameString(), roseOptions, unparseHelp, unparseDelegate );

       // Location to turn on unparser specific debugging data that shows up in the output file
       // This prevents the unparsed output file from compiling properly!
       // ROSE_DEBUG = 0;

       // DQ (12/5/2006): Output information that can be used to colorize properties of generated code (useful for debugging).
          roseUnparser.set_embedColorCodesInGeneratedCode ( file->get_embedColorCodesInGeneratedCode() );
          roseUnparser.set_generateSourcePositionCodes    ( file->get_generateSourcePositionCodes() );

       // information that is passed down through the tree (inherited attribute)
       // SgUnparse_Info inheritedAttributeInfo (NO_UNPARSE_INFO);
          SgUnparse_Info inheritedAttributeInfo;

       // Call member function to start the unparsing process
       // roseUnparser.run_unparser();
       // roseUnparser.unparseFile(file,inheritedAttributeInfo);

       // DQ (9/2/2008): This one way to handle the variations in type
          switch (file->variantT())
             {
               case V_SgSourceFile:
                  {
                    SgSourceFile* sourceFile = isSgSourceFile(file);
                    roseUnparser.unparseFile(sourceFile,inheritedAttributeInfo);
                    break;
                  }

               case V_SgBinaryFile:
                  {
                    SgBinaryFile* binaryFile = isSgBinaryFile(file);
                    roseUnparser.unparseFile(binaryFile,inheritedAttributeInfo);
                    break;
                  }

               case V_SgUnknownFile:
                  {
                    SgUnknownFile* unknownFile = isSgUnknownFile(file);

                    unknownFile->set_skipfinalCompileStep(true);

                    printf ("Warning: Unclear what to unparse from a SgUnknownFile (set skipfinalCompileStep) \n");
                    break;
                  }

               default:
                  {
                    printf ("Error: default reached in unparser: file = %s \n",file->class_name().c_str());
                    ROSE_ASSERT(false);
                  }
             }          

       // And finally we need to close the file (to flush everything out!)
          ROSE_OutputFile.close();
        }
   }
#endif

// DQ (10/11/2007): I think this is redundant with the Unparser::unparseProject() member function
// But it is allowed to call it directly from the user's translator if compilation using the backend 
// is not required!  So we have to allow it to be here.
void unparseProject( SgProject* project, UnparseFormatHelp *unparseFormatHelp, UnparseDelegate* unparseDelegate)
   {
     ROSE_ASSERT(project != NULL);
     for (int i=0; i < project->numberOfFiles(); ++i)
        {
          SgFile & file = project->get_file(i);
          unparseFile(&file,unparseFormatHelp,unparseDelegate);
        }
   }

