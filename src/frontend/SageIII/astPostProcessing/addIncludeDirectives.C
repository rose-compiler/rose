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

     return return_value;
   }

void addIncludeDirectives( SgNode* node )
   {
     TimingPerformance timer ("Add Include Directives:");
     std::map<std::string, SgIncludeFile*> & includeMap = EDG_ROSE_Translation::edg_include_file_map;

  // Assume the list of Include files are sorted by there source sequence ranges.
     std::list<SgIncludeFile*> sortedList;
     std::map<std::string, SgIncludeFile*>::iterator i = includeMap.begin();

     while (i != includeMap.end())
       {
         sortedList.push_back(i->second);
         i++;
       }

     sortedList.sort(source_sequence_compare);

     std::list<SgIncludeFile*>::iterator k = sortedList.begin();
     while (k != sortedList.end())
        {
          SgIncludeFile* include_file = *k;

       // Preinclude files are not added to the AST, since they are not structurally present in the source code).
          if (include_file->get_isPreinclude() == false)
             {
               AddIncludeDirectivesTraversal addIncludeDireclrationsTraversal (*k);

            // This inherited attribute is used for all traversals (within the iterative approach we define)
               AddIncludeDirectivesInheritedAttribute inheritedAttribute;

            // This will be called iteratively so that we can do a fixed point iteration
               addIncludeDireclrationsTraversal.traverse(node,inheritedAttribute);
             }
            else
             {
               printf (" #### pre-include files are not added to the AST \n");
             }
          k++;
        }
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
   }

AddIncludeDirectivesSynthesizedAttribute::AddIncludeDirectivesSynthesizedAttribute(const AddIncludeDirectivesSynthesizedAttribute &)
   {
   }

AddIncludeDirectivesTraversal::AddIncludeDirectivesTraversal(SgIncludeFile* input_include_file)
   : include_file(input_include_file)
   {
     previous_statement = nullptr;
     includeDirectiveHasBeenInserted = false;
   }

AddIncludeDirectivesInheritedAttribute
AddIncludeDirectivesTraversal::evaluateInheritedAttribute(SgNode* node, AddIncludeDirectivesInheritedAttribute /*inheritedAttribute*/)
   {
     ROSE_ASSERT(include_file != NULL);
     AddIncludeDirectivesInheritedAttribute return_attribute;

  // Handle statements only.
     SgStatement* statement = isSgStatement(node);

  // DQ (5/15/2020): We only want to insert the include directive once.
     if (includeDirectiveHasBeenInserted == false && statement != NULL)
        {
          Sg_File_Info* fileInfo = statement->get_startOfConstruct();
          bool insertIncludeDirective = false;

          if (fileInfo->get_source_sequence_number() > include_file->get_last_source_sequence_number())
             {
               SgIncludeFile* parent_include_file = include_file->get_parent_include_file();
               ROSE_ASSERT(parent_include_file != NULL);
               if (fileInfo->get_filenameString() == parent_include_file->get_filename().str())
                  {
                    insertIncludeDirective = true;
                  }
             }

          if (insertIncludeDirective == true)
             {
            // Insert the include directive at this statement.
               printf ("@@@@@@@@@@@@ Insert the include directive at this statement = %p = %s \n",statement,statement->class_name().c_str());
               Sg_File_Info* file_info = statement->get_startOfConstruct();
               printf (" --- statement at line %d in file        = %s \n",file_info->get_line(),file_info->get_filenameString().c_str());
               SgIncludeFile* parent_include_file = include_file->get_parent_include_file();
               ROSE_ASSERT(parent_include_file != NULL);
               printf (" --- parent_include_file->get_filename() = %s \n",parent_include_file->get_filename().str());

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

               includeDirectiveStatement->set_startOfConstruct(Sg_File_Info::generateDefaultFileInfoForTransformationNode());
               includeDirectiveStatement->set_endOfConstruct  (Sg_File_Info::generateDefaultFileInfoForTransformationNode());

            // DQ (8/24/2018): Connect this properly into the AST.
               includeDirectiveStatement->set_parent(statement->get_scope());

            // NOTE: Set, but not used in the current implementation.
               string name_used_in_include_directive = include_file->get_name_used_in_include_directive();

               string include_directive_string = string("#include \"") + name_used_in_include_directive + "\"";
               printf ("include_directive_string = %s \n",include_directive_string.c_str());

               includeDirectiveStatement->set_directiveString(include_directive_string);
               printf ("includeDirectiveStatement->get_directiveString() = %s \n",includeDirectiveStatement->get_directiveString().c_str());

            // DQ (11/5/2018): Pass the name that was used to the SgIncludeDirectiveStatement.
               includeDirectiveStatement->set_name_used_in_include_directive(include_file->get_name_used_in_include_directive());

               bool insertBefore              = true;
               bool autoMovePreprocessingInfo = true;
               SageInterface::insertStatement(statement,includeDirectiveStatement,insertBefore,autoMovePreprocessingInfo);

               includeDirectiveHasBeenInserted = true;
             }
        }

     return return_attribute;
   }

AddIncludeDirectivesSynthesizedAttribute
AddIncludeDirectivesTraversal::evaluateSynthesizedAttribute (
     SgNode* /*node*/,
     AddIncludeDirectivesInheritedAttribute /*inheritedAttribute*/,
     SubTreeSynthesizedAttributes /*synthesizedAttributeList*/)
   {
     ASSERT_not_null(include_file);
     AddIncludeDirectivesSynthesizedAttribute return_attribute;

     return return_attribute;
   }
