string globalUnparseToString ( SgNode* astNode, SgUnparse_Info* inputUnparseInfoPointer )
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

     int lineNumber = 0;  // Zero indicates that ALL lines should be unparsed

     // Initialize the Unparser using a special string stream inplace of the usual file stream 
     ostringstream outputString;

     SgLocatedNode* locatedNode = isSgLocatedNode(astNode);
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
       // fileNameOfStatementsToUnparse = rose::getFileNameByTraversalBackToFileNode(locatedNode);
          if (locatedNode->get_parent() == NULL)
             {
            // DQ (7/29/2005):
            // Allow this function to be called with disconnected AST fragments not connected to 
            // a previously generated AST.  This happens in Qing's interface where AST fragements 
            // are built and meant to be unparsed.  Only the parent of the root of the AST 
            // fragement is expected to be NULL.
               fileNameOfStatementsToUnparse = locatedNode->getFileName();
             }
            else
             {
               fileNameOfStatementsToUnparse = rose::getFileNameByTraversalBackToFileNode(locatedNode);
             }
          
        }
     ROSE_ASSERT (fileNameOfStatementsToUnparse.size() > 0);

     Unparser roseUnparser ( &outputString, fileNameOfStatementsToUnparse, roseOptions, lineNumber );

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
        }

     ROSE_ASSERT (inheritedAttributeInfoPointer != NULL);
     SgUnparse_Info & inheritedAttributeInfo = *inheritedAttributeInfoPointer;

  // Turn ON the error checking which triggers an error if the default SgUnparse_Info constructor is called
  // SgUnparse_Info::forceDefaultConstructorToTriggerError = true;

#if 1
  // DQ (10/19/2004): Cleaned up this code, remove this dead code after we are sure that this worked properly
  // Actually, this code is required to be this way, since after this branch the current function returns and
  // some data must be cleaned up differently!  So put this back and leave it this way, and remove the
  // "Implementation Note".

  // Both SgProject and SgFile are handled via recursive calls
     if ( (isSgProject(astNode) != NULL) || (isSgFile(astNode) != NULL) )
        {
       // printf ("Implementation Note: Put these cases (unparsing the SgProject and SgFile into the cases for nodes derived from SgSupport below! \n");

       // Handle recursive call for SgProject
          if (isSgProject(astNode) != NULL)
             {
               SgProject* project = isSgProject(astNode);
               ROSE_ASSERT(project != NULL);
               for (int i = 0; i < project->numberOfFiles(); i++)
                  {
                    SgFile* file = &(project->get_file(i));
                    ROSE_ASSERT(file != NULL);
                    string unparsedFileString = globalUnparseToString(file,inputUnparseInfoPointer);
                    string prefixString       = string("/* TOP:")      + string(rose::getFileName(file)) + string(" */ \n");
                    string suffixString       = string("\n/* BOTTOM:") + string(rose::getFileName(file)) + string(" */ \n\n");
                    returnString += prefixString + unparsedFileString + suffixString;
                  }
             }

       // Handle recursive call for SgFile
          if (isSgFile(astNode) != NULL)
             {
               SgFile* file = isSgFile(astNode);
               ROSE_ASSERT(file != NULL);
               SgGlobal* globalScope = file->get_root();
               ROSE_ASSERT(globalScope != NULL);
               returnString = globalUnparseToString(globalScope,inputUnparseInfoPointer);
             }
        }
       else
#endif
        {
       // DQ (1/12/2003): Only now try to trap use of SgUnparse_Info default constructor
       // Turn ON the error checking which triggers an error if the default SgUnparse_Info constructor is called
          SgUnparse_Info::set_forceDefaultConstructorToTriggerError(true);

          if (isSgStatement(astNode) != NULL)
             {
               SgStatement* stmt = isSgStatement(astNode);
               roseUnparser.unparseStatement ( stmt, inheritedAttributeInfo );
             }

          if (isSgExpression(astNode) != NULL)
             {
               SgExpression* expr = isSgExpression(astNode);
               roseUnparser.unparseExpression ( expr, inheritedAttributeInfo );
             }

          if (isSgType(astNode) != NULL)
             {
               SgType* type = isSgType(astNode);
               roseUnparser.unparseType ( type, inheritedAttributeInfo );
             }

          if (isSgSymbol(astNode) != NULL)
             {
               SgSymbol* symbol = isSgSymbol(astNode);
               roseUnparser.unparseSymbol ( symbol, inheritedAttributeInfo );
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
                              string unparsedFileString = globalUnparseToString(file,inputUnparseInfoPointer);
                              string prefixString       = string("/* TOP:")      + string(rose::getFileName(file)) + string(" */ \n");
                              string suffixString       = string("\n/* BOTTOM:") + string(rose::getFileName(file)) + string(" */ \n\n");
                              returnString += prefixString + unparsedFileString + suffixString;
                            }
                         break;
                       }
                    case V_SgFile:
                       {
                         SgFile* file = isSgFile(astNode);
                         ROSE_ASSERT(file != NULL);
                         SgGlobal* globalScope = file->get_root();
                         ROSE_ASSERT(globalScope != NULL);
                         returnString = globalUnparseToString(globalScope,inputUnparseInfoPointer);
                         break;
                       }
#endif
                    case V_SgTemplateParameter:
                       {
                         SgTemplateParameter* templateParameter = isSgTemplateParameter(astNode);
                         roseUnparser.unparseTemplateParameter(templateParameter,inheritedAttributeInfo);
                         break;
                       }
                    case V_SgTemplateArgument:
                       {
                         SgTemplateArgument* templateArgument = isSgTemplateArgument(astNode);
                         roseUnparser.unparseTemplateArgument(templateArgument,inheritedAttributeInfo);
                         break;
                       }
                    case V_SgInitializedName:
                       {
                      // QY: not sure how to implement this
                      // DQ (7/23/2004): This should unparse as a declaration 
                      // (type and name with initializer).
                         break;
                       }

                    case V_Sg_File_Info:
                       {
                      // DQ (5/11/2006): Not sure how or if we shoul implement this
                         break;
                       }

                 // Perhaps the support for SgFile and SgProject shoud be moved to this location?
                    default:
                         printf ("Error: default reached in node derived from SgSupport astNode = %s \n",astNode->sage_class_name());
                         ROSE_ABORT();
                }
             }

       // Turn OFF the error checking which triggers an if the default SgUnparse_Info constructor is called
          SgUnparse_Info::set_forceDefaultConstructorToTriggerError(false);

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

// Later we might want to move this to the SgProject or SgFile support class (generated by ROSETTA)
void
// unparseFile ( SgFile & file, UnparseFormatHelp *unparseHelp, UnparseDelegate* repl  )
unparseFile ( SgFile* file, UnparseFormatHelp *unparseHelp, UnparseDelegate* unparseDelegate )
   {
  // Call the unparser mechanism
     ROSE_ASSERT(file != NULL);
  // DQ (4/22/2006): This can be true when the "-E" option is used, but then we should not have called unparse()!
     ROSE_ASSERT(file->get_skip_unparse() == false);

  // If we did unparse an intermediate file then we want to compile that 
  // file instead of the original source file.
     if (file->get_unparse_output_filename().empty() == true)
        {
          string outputFilename = "rose_" + file->get_sourceFileNameWithoutPath();
          file->set_unparse_output_filename(outputFilename);
          ROSE_ASSERT (file->get_unparse_output_filename().empty() == false);
       // printf ("Inside of SgFile::unparse(UnparseFormatHelp*,UnparseDelegate*) outputFilename = %s \n",outputFilename.c_str());
        }

     if (file->get_skip_unparse() == true)
        {
       // MS: commented out the following output
        }
       else
        {
       // Open the file where we will put the generated code
          string outputFilename = get_output_filename(*file);
          fstream ROSE_OutputFile(outputFilename.c_str(),ios::out);
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

       // control unparsing of include files into the source file (default is false)
          bool _unparse_includes             = file->get_unparse_includes();

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

          int lineNumber = 0;  // Zero indicates that ALL lines should be unparsed

          Unparser roseUnparser ( &ROSE_OutputFile, file->get_file_info()->get_filenameString(), roseOptions, lineNumber, unparseHelp, unparseDelegate );

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
          roseUnparser.unparseFile(file,inheritedAttributeInfo);

       // And finally we need to close the file (to flush everything out!)
          ROSE_OutputFile.close();
        }
   }


void unparseProject( SgProject* project, UnparseFormatHelp *unparseFormatHelp, UnparseDelegate* unparseDelegate)
   {
     ROSE_ASSERT(project != NULL);
     for (int i=0; i < project->numberOfFiles(); ++i)
        {
          SgFile & file = project->get_file(i);
          unparseFile(&file,unparseFormatHelp,unparseDelegate);
        }
   }


