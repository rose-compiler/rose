

// DQ (5/7/2020): Iterate of the AST and add the include directives associated with 
// SgIncludeFiles that were collected in the generation of the AST.

#include "sage3basic.h"
#include "addIncludeDirectives.h"

using namespace std;


static bool source_sequence_compare (SgIncludeFile* first, SgIncludeFile* second)
   {
     bool return_value = false;

     if (first->get_first_source_sequence_number() < second->get_first_source_sequence_number())
        {
          return_value = true;
        }
       else
        {
          if (first->get_first_source_sequence_number() == second->get_first_source_sequence_number())
             {
               if (first->get_last_source_sequence_number() < second->get_last_source_sequence_number())
                  {
                    return_value = true;
                  }
                 else
                  {
                    return_value = false;
                  }
             }
        }

#if 0
     printf ("In source_sequence_compare(): \n");
     printf (" --- first  = %p = %s \n",first,first->get_filename().str());
     printf (" --- --- first->get_first_source_sequence_number() = %u \n",first->get_first_source_sequence_number());
     printf (" --- --- first->get_last_source_sequence_number()  = %u \n",first->get_last_source_sequence_number());
     printf (" --- second = %p = %s \n",second,second->get_filename().str());
     printf (" --- --- second->get_first_source_sequence_number() = %u \n",second->get_first_source_sequence_number());
     printf (" --- --- second->get_last_source_sequence_number()  = %u \n",second->get_last_source_sequence_number());
     printf (" --- return_value = %s \n",return_value ? "true" : "false");
#endif

     return return_value;
   }


void addIncludeDirectives( SgNode* node )
   {

  // DQ (7/7/2005): Introduce tracking of performance of ROSE.
     TimingPerformance timer ("Add Include Directives:");

     std::map<std::string, SgIncludeFile*> & includeMap = EDG_ROSE_Translation::edg_include_file_map;

#if 0
     printf ("In addIncludeDirectives(): includeMap.size() = %zu \n",includeMap.size());
#endif

  // Assume the list of Include files are sorted by there source sequence ranges.
  // std::list<SgIncludeFile*> sortedList(include_file_map.begin(),include_file_map.end());
  // std::vector<SgIncludeFile*> sortedList;
     std::list<SgIncludeFile*> sortedList;
     std::map<std::string, SgIncludeFile*>::iterator i = includeMap.begin();

     while (i != includeMap.end())
       {
         sortedList.push_back(i->second);
         i++;
       }

#if 0
     printf ("sortedList.size() = %zu \n",sortedList.size());
#endif
#if 0
     printf ("Exiting as a test before addIncludeDirectives traversal \n");
     ROSE_ASSERT(false);
#endif

#if 0
     std::list<SgIncludeFile*>::iterator j = sortedList.begin();

     printf ("Unsorted list: \n");
     while (j != sortedList.end())
        {
          SgIncludeFile* tmp_include_file = *j;
          printf (" --- include file = %p = %s \n",tmp_include_file,tmp_include_file->get_filename().str());
          printf (" --- --- get_first_source_sequence_number() = %u \n",tmp_include_file->get_first_source_sequence_number());
          printf (" --- --- get_last_source_sequence_number()  = %u \n",tmp_include_file->get_last_source_sequence_number());

          j++;
        }
#endif

#if 0
     printf ("Exiting as a test before addIncludeDirectives traversal \n");
     ROSE_ASSERT(false);
#endif

#if 0
     printf ("Sort the include file list \n");
#endif

     sortedList.sort(source_sequence_compare);

#if 0
     printf ("Sorted list: \n");
     j = sortedList.begin();
     while (j != sortedList.end())
        {
          SgIncludeFile* tmp_include_file = *j;
          printf (" --- include file = %p = %s \n",tmp_include_file,tmp_include_file->get_filename().str());
          printf (" --- --- get_first_source_sequence_number() = %u \n",tmp_include_file->get_first_source_sequence_number());
          printf (" --- --- get_last_source_sequence_number()  = %u \n",tmp_include_file->get_last_source_sequence_number());

          j++;
        }
#endif
#if 0
     printf ("Exiting as a test before addIncludeDirectives traversal \n");
     ROSE_ASSERT(false);
#endif


  // std::map<std::string, SgIncludeFile*>::iterator j = includeMap.begin();
     std::list<SgIncludeFile*>::iterator k = sortedList.begin();
     while (k != sortedList.end())
        {
          SgIncludeFile* include_file = *k;
#if 0
          printf ("@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@ \n");
          printf ("supporting include file: %s \n",include_file->get_filename().str());
          printf ("@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@ \n");
#endif

       // Preinclude files are not added to the AST, since they are not structurally present in the source code).
          if (include_file->get_isPreinclude() == false)
             {
               AddIncludeDirectivesTraversal addIncludeDireclrationsTraversal (*k);

            // This inherited attribute is used for all traversals (within the iterative approach we define)
               AddIncludeDirectivesInheritedAttribute inheritedAttribute;

            // This will be called iteratively so that we can do a fixed point iteration
               addIncludeDireclrationsTraversal.traverse(node,inheritedAttribute);
#if 0
               printf ("Exiting as a test: Finished with file: %s \n",include_file->get_filename().str());
               ROSE_ASSERT(false);
#endif
             }
            else
             {
#if 1
               printf (" #### pre-include files are not added to the AST \n");
#endif
             }

#if 0
          printf ("END OF LOOP: Exiting as a test after addIncludeDirectives traversal \n");
          ROSE_ASSERT(false);
#endif
          k++;
        }

#if 0
     printf ("Exiting as a test after addIncludeDirectives traversal \n");
     ROSE_ASSERT(false);
#endif
   }



AddIncludeDirectivesInheritedAttribute::AddIncludeDirectivesInheritedAttribute() : xxx(false)
   {
     foundStatementFromIncludeFile = false;
   }

AddIncludeDirectivesInheritedAttribute::AddIncludeDirectivesInheritedAttribute( const AddIncludeDirectivesInheritedAttribute & X )
   {
     foundStatementFromIncludeFile = X.foundStatementFromIncludeFile;
   }


AddIncludeDirectivesSynthesizedAttribute::AddIncludeDirectivesSynthesizedAttribute()
   {
#if 0
     printf ("In AddIncludeDirectivesSynthesizedAttribute constructor: node = %p = %s \n",node,node->class_name().c_str());
#endif
   }

AddIncludeDirectivesSynthesizedAttribute::AddIncludeDirectivesSynthesizedAttribute( const AddIncludeDirectivesSynthesizedAttribute & X )
   {
   }


#if 0
AddIncludeDirectivesTraversal::AddIncludeDirectivesTraversal(std::map<std::string, SgIncludeFile*> & input_include_file_map)
   : include_file_map(input_include_file_map)
   {
     printf ("In AddIncludeDirectivesTraversal constructor: include_file_map.size() = %zu \n",include_file_map.size());
   }
#endif

// AddIncludeDirectivesTraversal::AddIncludeDirectivesTraversal(SgIncludeFile* input_include_file)
//   : include_file(input_include_file)
AddIncludeDirectivesTraversal::AddIncludeDirectivesTraversal(SgIncludeFile* input_include_file)
   : include_file(input_include_file)
   {
#if 0
     printf ("In AddIncludeDirectivesTraversal constructor: include_file->get_filename() = %s \n",include_file->get_filename().str());
#endif

     previous_statement              = NULL;
     includeDirectiveHasBeenInserted = false;
   }


AddIncludeDirectivesInheritedAttribute
AddIncludeDirectivesTraversal::evaluateInheritedAttribute ( SgNode* node, AddIncludeDirectivesInheritedAttribute inheritedAttribute )
   {
#if 0
     printf ("\nIn AddIncludeDirectivesTraversal::evaluateInheritedAttribute: node = %p = %s \n",node,node->class_name().c_str());
#endif

  // SgIncludeFile* include_file = inheritedAttribute->include_file;
     ROSE_ASSERT(include_file != NULL);

  // AddIncludeDirectivesInheritedAttribute return_attribute(include_file);
     AddIncludeDirectivesInheritedAttribute return_attribute;

  // Initialize the return attribute.
  // return_attribute.foundStatementFromIncludeFile = inheritedAttribute.foundStatementFromIncludeFile;

  // Handle statements only.
     SgStatement* statement = isSgStatement(node);

  // DQ (5/15/2020): We only want to insert the include directive once.
  // if (statement != NULL)
     if (includeDirectiveHasBeenInserted == false && statement != NULL)
        {
#if 0
          printf ("evaluateInheritedAttribute: statement = %p = %s \n",statement,statement->class_name().c_str());
#endif
          Sg_File_Info* fileInfo = statement->get_startOfConstruct();
#if 0
       // if (inheritedAttribute.foundStatementFromIncludeFile == false)
             {
               printf (" --- evaluateInheritedAttribute: statement at line %d in file = %s \n",fileInfo->get_line(),fileInfo->get_filenameString().c_str());
               printf (" --- associated source sequence point = %u \n",fileInfo->get_source_sequence_number());

               printf (" --- include file = %p = %s \n",include_file,include_file->get_filename().str());
               printf (" --- --- get_first_source_sequence_number() = %u \n",include_file->get_first_source_sequence_number());
               printf (" --- --- get_last_source_sequence_number()  = %u \n",include_file->get_last_source_sequence_number());

               printf (" --- --- inheritedAttribute.foundStatementFromIncludeFile = %s \n",inheritedAttribute.foundStatementFromIncludeFile ? "true" : "true");
             }
#endif
          bool insertIncludeDirective = false;

       // if (inheritedAttribute.foundStatementFromIncludeFile == true)
       // if (includeDirectiveHasBeenInserted == false && fileInfo->get_source_sequence_number() > include_file->get_last_source_sequence_number())
          if (fileInfo->get_source_sequence_number() > include_file->get_last_source_sequence_number())
             {
               SgIncludeFile* parent_include_file = include_file->get_parent_include_file();
               ROSE_ASSERT(parent_include_file != NULL);
               if (fileInfo->get_filenameString() == parent_include_file->get_filename().str())
                  {
                    insertIncludeDirective = true;
#if 0
                    printf ("Setting insertIncludeDirective = true \n");
#endif
#if 0
                    printf ("Exiting as a test: Setting insertIncludeDirective = true \n");
                    ROSE_ASSERT(false);
#endif
                  }
             }
            else
             {
               if (fileInfo->get_filenameString() == include_file->get_filename().str())
                  {
#if 0
                    printf ("Found statement from target include file = %s \n",include_file->get_filename().str());
#endif
                 // return_attribute.foundStatementFromIncludeFile = true;
#if 0
                    printf ("Exiting as a test: Found statement from target include file \n");
                    ROSE_ASSERT(false);
#endif
                  }
             }

       // if (fileInfo->get_source_sequence_number() > include_file->get_last_source_sequence_number())
          if (insertIncludeDirective == true)
             {
            // Insert the include directive at this statement.
               printf ("@@@@@@@@@@@@ Insert the include directive at this statement = %p = %s \n",statement,statement->class_name().c_str());
               Sg_File_Info* file_info = statement->get_startOfConstruct();
               printf (" --- statement at line %d in file        = %s \n",file_info->get_line(),file_info->get_filenameString().c_str());
               SgIncludeFile* parent_include_file = include_file->get_parent_include_file();
               ROSE_ASSERT(parent_include_file != NULL);
               printf (" --- parent_include_file->get_filename() = %s \n",parent_include_file->get_filename().str());
#if 0
               printf (" --- include_file->get_filename()               = %s \n",include_file->get_filename().str());
               printf (" --- include_file->get_isIncludedMoreThanOnce() = %s \n",include_file->get_isIncludedMoreThanOnce() ? "true" : "false");
               printf (" --- include_file->get_isPrimaryUse()           = %s \n",include_file->get_isPrimaryUse() ? "true" : "false");
               printf (" --- include_file->get_isPreinclude()           = %s \n",include_file->get_isPreinclude() ? "true" : "false");
#endif

            // Preinclude files are not added to the AST, since they are not structurally present in the source code).
               ROSE_ASSERT(include_file->get_isPreinclude() == false);

            // Adding SgIncludeDirectiveStatement
               SgIncludeDirectiveStatement* includeDirectiveStatement = new SgIncludeDirectiveStatement();
               ROSE_ASSERT(includeDirectiveStatement != NULL);

            // DQ (6/3/2019): To be consistant with other SgDeclarationStatement IR nodes, mark this as the defining declaration.
               includeDirectiveStatement->set_definingDeclaration(includeDirectiveStatement);
               ROSE_ASSERT(includeDirectiveStatement->get_definingDeclaration() != NULL);

            // DQ (9/18/2018): Adding the connection to the include file hierarchy as generated in the EDG/ROSE translation.
               includeDirectiveStatement->set_include_file_heirarchy(include_file);

            // DQ (11/9/2018): Not clear if this is the best solution.  Might be better to have 
            // it point to the SgSourceFile or the parent include file IR node.
            // include_file->set_parent(includeDirectiveStatement);
#if 0
               printf ("Attaching CPP directives: include_file->get_name_used_in_include_directive() = %s \n",include_file->get_name_used_in_include_directive().str());
#endif
            // DQ (8/21/2018): Set a better source file position for the SgIncludeDirectiveStatement.
            // includeDirectiveStatement->set_startOfConstruct(new Sg_File_Info(*(currentPreprocessingInfoPtr-> get_file_info())));
            // includeDirectiveStatement->set_endOfConstruct  (new Sg_File_Info(*(currentPreprocessingInfoPtr-> get_file_info())));
               includeDirectiveStatement->set_startOfConstruct(Sg_File_Info::generateDefaultFileInfoForTransformationNode());
               includeDirectiveStatement->set_endOfConstruct  (Sg_File_Info::generateDefaultFileInfoForTransformationNode());

            // DQ (8/24/2018): Connect this properly into the AST.
               includeDirectiveStatement->set_parent(statement->get_scope());

            // NOTE: Set, but not used in the current implementation.
               string name_used_in_include_directive = include_file->get_name_used_in_include_directive();

               string include_directive_string = string("#include \"") + name_used_in_include_directive + "\"";
#if 1
               printf ("include_directive_string = %s \n",include_directive_string.c_str());
#endif
            // includeDirectiveStatement->set_directiveString(name_used_in_include_directive);
               includeDirectiveStatement->set_directiveString(include_directive_string);
#if 1
               printf ("includeDirectiveStatement->get_directiveString() = %s \n",includeDirectiveStatement->get_directiveString().c_str());
#endif
            // DQ (11/5/2018): Pass the name that was used to the SgIncludeDirectiveStatement.
               includeDirectiveStatement->set_name_used_in_include_directive(include_file->get_name_used_in_include_directive());

               bool insertBefore              = true;
               bool autoMovePreprocessingInfo = true;
               SageInterface::insertStatement(statement,includeDirectiveStatement,insertBefore,autoMovePreprocessingInfo);
#if 0
               printf ("Exiting as a test: Insert the include directive at this statement \n");
               ROSE_ASSERT(false);
#endif
               includeDirectiveHasBeenInserted = true;
             }

       // Save the previous statement
       // previous_statement = statement;
        }

     return return_attribute;
   }


AddIncludeDirectivesSynthesizedAttribute
AddIncludeDirectivesTraversal::evaluateSynthesizedAttribute (
     SgNode* node,
     AddIncludeDirectivesInheritedAttribute inheritedAttribute,
     SubTreeSynthesizedAttributes synthesizedAttributeList )
   {
#if 0
     printf ("In AddIncludeDirectivesTraversal::evaluateSynthesizedAttribute: node = %p = %s \n",node,node->class_name().c_str());
#endif
     ROSE_ASSERT(include_file != NULL);

     AddIncludeDirectivesSynthesizedAttribute return_attribute;

     SgStatement* statement = isSgStatement(node);
     if (statement != NULL)
        {
#if 0
          printf ("evaluateSynthesizedAttribute:statement = %p = %s \n",statement,statement->class_name().c_str());
          Sg_File_Info* fileInfo = statement->get_startOfConstruct();
          printf (" --- associated source sequence point = %u \n",fileInfo->get_source_sequence_number()); 
#endif
        }

     return return_attribute;
   }
