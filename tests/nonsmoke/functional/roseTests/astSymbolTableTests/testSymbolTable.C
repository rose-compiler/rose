#include"rose.h"

void lookup_named_symbols (SgGlobal* globalScope, const SgName & name)
   {
     SgTemplateParameterPtrList* templateParameterList = NULL;
     SgTemplateArgumentPtrList*  templateArgumentList  = NULL;

  // DQ (8/21/2013): It does not make sense to NOT know what symbol your looking for...
     SgSymbol* symbol = globalScope->lookup_symbol(name,templateParameterList,templateArgumentList);
     while (symbol != NULL)
        {
          printf ("symbol = %p = %s \n",symbol,symbol->get_name().str());
          symbol = globalScope->next_any_symbol();
        }

     SgVariableSymbol* varSymbol = globalScope->lookup_variable_symbol(name);
     while (varSymbol != NULL)
        {
          printf ("varSymbol = %p = %s \n",varSymbol,varSymbol->get_name().str());
          varSymbol = globalScope->next_variable_symbol();
        }

     SgClassSymbol* classSymbol = globalScope->lookup_class_symbol(name);
     while (classSymbol != NULL)
        {
          printf ("classSymbol = %p = %s \n",classSymbol,classSymbol->get_name().str());
          classSymbol = globalScope->next_class_symbol();
        }

     SgFunctionSymbol* functionSymbol = globalScope->lookup_function_symbol(name);
     while (functionSymbol != NULL)
        {
          printf ("functionSymbol = %p = %s \n",functionSymbol,functionSymbol->get_name().str());
          functionSymbol = globalScope->next_function_symbol();
        }

     SgTypedefSymbol* typedefSymbol = globalScope->lookup_typedef_symbol(name);
     while (typedefSymbol != NULL)
        {
          printf ("typedefSymbol = %p = %s \n",typedefSymbol,typedefSymbol->get_name().str());
          typedefSymbol = globalScope->next_typedef_symbol();
        }

     SgEnumSymbol* enumSymbol = globalScope->lookup_enum_symbol(name);
     while (enumSymbol != NULL)
        {
          printf ("enumSymbol = %p = %s \n",enumSymbol,enumSymbol->get_name().str());
          enumSymbol = globalScope->next_enum_symbol();
        }

     SgEnumFieldSymbol* enumFieldSymbol = globalScope->lookup_enum_field_symbol(name);
     while (enumFieldSymbol != NULL)
        {
          printf ("enumFieldSymbol = %p = %s \n",enumFieldSymbol,enumFieldSymbol->get_name().str());
          enumFieldSymbol = globalScope->next_enum_field_symbol();
        }

     SgLabelSymbol* labelSymbol = globalScope->lookup_label_symbol(name);
     while (labelSymbol != NULL)
        {
          printf ("labelSymbol = %p = %s \n",labelSymbol,labelSymbol->get_name().str());
          labelSymbol = globalScope->next_label_symbol();
        }

     SgNamespaceSymbol* namespaceSymbol = globalScope->lookup_namespace_symbol(name);
     while (namespaceSymbol != NULL)
        {
          printf ("namespaceSymbol = %p = %s \n",namespaceSymbol,namespaceSymbol->get_name().str());
          namespaceSymbol = globalScope->next_namespace_symbol();
        }

#if 0
  // DQ (8/21/2013): This function does not make since any more because the latest branch of ROSE (EDG4x) redesigns the template handling).
     SgTemplateSymbol* templateSymbol = globalScope->lookup_template_symbol(name);
     while (templateSymbol != NULL)
        {
          printf ("templateSymbol = %p = %s \n",templateSymbol,templateSymbol->get_name().str());
          templateSymbol = globalScope->next_template_symbol();
        }
#endif
#if 0
  // DQ (8/21/2013): These symbol lookup functions require template parameters and template arguments.
  // However, there are not next_XXX() versions of these functions yet.
     SgType* type = NULL;

     SgTemplateClassSymbol* templateClassSymbol = globalScope->lookup_template_class_symbol(name,templateParameterList,templateArgumentList);
     while (templateClassSymbol != NULL)
        {
          printf ("templateClassSymbol = %p = %s \n",templateClassSymbol,templateClassSymbol->get_name().str());
          templateClassSymbol = globalScope->next_template_class_symbol();
        }

     SgTemplateFunctionSymbol* templateFunctionSymbol = globalScope->lookup_template_function_symbol(name,type,templateParameterList);
     while (templateFunctionSymbol != NULL)
        {
          printf ("templateFunctionSymbol = %p = %s \n",templateFunctionSymbol,templateFunctionSymbol->get_name().str());
          templateFunctionSymbol = globalScope->next_template_function_symbol();
        }

     SgTemplateMemberFunctionSymbol* templateMemberFunctionSymbol = globalScope->lookup_template_member_function_symbol(name,type,templateParameterList);
     while (templateMemberFunctionSymbol != NULL)
        {
          printf ("templateMemberFunctionSymbol = %p = %s \n",templateMemberFunctionSymbol,templateMemberFunctionSymbol->get_name().str());
          templateMemberFunctionSymbol = globalScope->next_template_member_function_symbol();
        }
#endif
   }


void
lookup_all_symbols(SgGlobal* globalScope)
   {
     SgSymbol* symbol = globalScope->first_any_symbol();
     while (symbol != NULL)
        {
          printf ("symbol = %p = %s \n",symbol,symbol->get_name().str());
          symbol = globalScope->next_any_symbol();
        }

     SgVariableSymbol* varSymbol = globalScope->first_variable_symbol();
     while (varSymbol != NULL)
        {
          printf ("varSymbol = %p = %s \n",varSymbol,varSymbol->get_name().str());
          varSymbol = globalScope->next_variable_symbol();
        }

     SgClassSymbol* classSymbol = globalScope->first_class_symbol();
     while (classSymbol != NULL)
        {
          printf ("classSymbol = %p = %s \n",classSymbol,classSymbol->get_name().str());
          classSymbol = globalScope->next_class_symbol();
        }

     SgFunctionSymbol* functionSymbol = globalScope->first_function_symbol();
     while (functionSymbol != NULL)
        {
          printf ("functionSymbol = %p = %s \n",functionSymbol,functionSymbol->get_name().str());
          functionSymbol = globalScope->next_function_symbol();
        }

     SgTypedefSymbol* typedefSymbol = globalScope->first_typedef_symbol();
     while (typedefSymbol != NULL)
        {
          printf ("typedefSymbol = %p = %s \n",typedefSymbol,typedefSymbol->get_name().str());
          typedefSymbol = globalScope->next_typedef_symbol();
        }

     SgEnumSymbol* enumSymbol = globalScope->first_enum_symbol();
     while (enumSymbol != NULL)
        {
          printf ("enumSymbol = %p = %s \n",enumSymbol,enumSymbol->get_name().str());
          enumSymbol = globalScope->next_enum_symbol();
        }

     SgEnumFieldSymbol* enumFieldSymbol = globalScope->first_enum_field_symbol();
     while (enumFieldSymbol != NULL)
        {
          printf ("enumFieldSymbol = %p = %s \n",enumFieldSymbol,enumFieldSymbol->get_name().str());
          enumFieldSymbol = globalScope->next_enum_field_symbol();
        }

     SgLabelSymbol* labelSymbol = globalScope->first_label_symbol();
     while (labelSymbol != NULL)
        {
          printf ("labelSymbol = %p = %s \n",labelSymbol,labelSymbol->get_name().str());
          labelSymbol = globalScope->next_label_symbol();
        }

     SgNamespaceSymbol* namespaceSymbol = globalScope->first_namespace_symbol();
     while (namespaceSymbol != NULL)
        {
          printf ("namespaceSymbol = %p = %s \n",namespaceSymbol,namespaceSymbol->get_name().str());
          namespaceSymbol = globalScope->next_namespace_symbol();
        }

#if 0
  // DQ (8/21/2013): This function does not make since any more because the latest branch of ROSE (EDG4x) redesigns the template handling).
     SgTemplateSymbol* templateSymbol = globalScope->first_template_symbol();
     while (templateSymbol != NULL)
        {
          printf ("templateSymbol = %p = %s \n",templateSymbol,templateSymbol->get_name().str());
          templateSymbol = globalScope->next_template_symbol();
        }
#endif
   }




int
main ( int argc, char* argv[] )
   {
     SgProject* project = frontend(argc,argv);
     ROSE_ASSERT (project != NULL);

  // SgGlobal* globalScope = project->get_file(0).get_globalScope();
  // SgGlobal* globalScope = project->operator[](0)->get_globalScope();
     SgSourceFile* sourceFile = isSgSourceFile(project->get_fileList()[0]);
     ROSE_ASSERT(sourceFile != NULL);

     SgGlobal* globalScope = sourceFile->get_globalScope();
     ROSE_ASSERT(globalScope != NULL);

#if 0
     printf ("********************* OUTPUT SYMBOL TABLE (START) *********************\n");
     SageInterface::outputLocalSymbolTables(globalScope);
     printf ("********************* OUTPUT SYMBOL TABLE (END) ***********************\n");
#endif

     lookup_all_symbols(globalScope);

     SgName name = "";
     lookup_named_symbols(globalScope,name);

     return backend(project);
   }
