#include "sage3basic.h"
#include "ModuleBuilder.h"

namespace Rose {

using namespace Rose::Diagnostics;
namespace fs = boost::filesystem;

void ModuleBuilder::setCurrentProject(SgProject* project)
{
  currentProject = project;
}

std::string ModuleBuilder::findFileFromInputDirs(const std::string &basename)
{
   // First look for module file in search path
   for (auto dir : inputDirs) {
     std::string file{dir + "/" + basename};
     if (fs::exists(file) && fs::is_regular_file(file)) {
       return file;
     }
   }

   // Next look for any regular file in search path (usage similar to Java CLASSPATH)
   for (auto file : inputDirs) {
     if (fs::exists(file) && fs::is_regular_file(file)) {
       return file;
     }
   }

   return basename;
}

// Convert to lower case and remove Jovial's extraneous tick (') marks
std::string ModuleBuilder::namespaceSymbolName(const std::string &name)
{
   std::string symbolName;
   symbolName.reserve(name.size());

   for (auto ch: name) {
     if (ch != '\'') symbolName += ch;
   }
   std::transform(symbolName.cbegin(), symbolName.cend(), symbolName.begin(), ::tolower);

   return symbolName;
}

void ModuleBuilder::setInputDirs(SgProject* project)
{
  std::vector<std::string> args{project->get_originalCommandLineArgumentList()};
  std::string rmodDir;

  // Add path to iso_c_binding.rmod. The intrinsic modules have been placed in the
  // 3rdPartyLibraries because they could be compiler dependent. If placed there we could
  // reasonable have multiple versions at some point.
  //
  // WARNING - this is for Fortran but really not needed for Jovial as no intrinsic compool modules
  //
  std::string intrinsic_mod_path = findRoseSupportPathFromSource("src/3rdPartyLibraries/fortran-parser", "share/rose");
  inputDirs.push_back(intrinsic_mod_path);

  for (unsigned i = 0; i < args.size(); i++) {
    if (args[i].find("-I",0) == 0) {
      rmodDir = args[i].substr(2);
      std::string rmodDir_no_quotes = boost::replace_all_copy(rmodDir, "\"", "");

      if (fs::exists(rmodDir_no_quotes.c_str())) {
        inputDirs.push_back(rmodDir_no_quotes);
      }
      else {
        if (Rose::ir_node_mlog[Rose::Diagnostics::DEBUG]) {
          mlog[WARN] << "the input directory does not exist (rose): " << rmodDir;
        }
      }
    }
  }
}

void ModuleBuilder::loadModule(const std::string &module_name, std::vector<std::string> &import_names, SgGlobal* file_scope)
{
  SgNamespaceDeclarationStatement* namespace_decl{nullptr};
  SgNamespaceDefinitionStatement*  namespace_defn{nullptr};
  SgNamespaceSymbol* namespace_symbol{nullptr};
  SgSymbolTable* namespace_symbols{nullptr};

  SgSourceFile* source = getModule(module_name);
  ASSERT_not_null(source);

  // A loaded module doesn't need to be unparsed or compiled in the back-end
  source->set_skip_unparse(true);
  source->set_skipfinalCompileStep(true);

  SgGlobal* module_scope = source->get_globalScope();
  ASSERT_not_null(module_scope);

  namespace_symbol = module_scope->lookup_namespace_symbol(module_name);
  if (!namespace_symbol) {
    std::string symbolName = namespaceSymbolName(module_name);

    // Check for alternate/mangled symbol names (Jovial may have extraneous ' marks)
    for (auto decl: module_scope->get_declarations()) {
      auto nameDecl = isSgNamespaceDeclarationStatement(decl);
      if (symbolName == namespaceSymbolName(nameDecl->get_name())) {
        namespace_symbol = module_scope->lookup_namespace_symbol(nameDecl->get_name());
        break;
      }
    }
  }

  if (namespace_symbol) namespace_decl = namespace_symbol->get_declaration();
  if (namespace_decl) namespace_defn = namespace_decl->get_definition();
  if (namespace_defn) namespace_symbols = namespace_defn->get_symbol_table();

  // inject namespace symbols into the file_scope of the caller
  if (namespace_symbols) {
    ASSERT_not_null(file_scope);
    for (SgNode* node : namespace_symbols->get_symbols()) {
      SgSymbol* symbol = isSgSymbol(node);

      if (SgAliasSymbol* alias = isSgAliasSymbol(symbol)) {
        symbol = alias->get_base();
      }
      ASSERT_not_null(symbol);

      SgName symbol_name = symbol->get_name();
      // Don't import symbols that already exist
      if (!file_scope->symbol_exists(symbol_name)) {
        if (import_names.size() == 0) {
          // Import all namespace symbols
          loadSymbol(symbol, namespace_symbols, file_scope);
        }
        else {
          // Only import names from the list, use lower case for comparison as Jovial and Fortran are case insensitive
          std::string lc_name{symbol_name};
          std::transform(lc_name.begin(), lc_name.end(), lc_name.begin(), ::tolower);

          std::vector<std::string>::iterator it = find (import_names.begin(), import_names.end(), lc_name);
          if (it != import_names.end()) {
            mlog[DEBUG] <<"inserting symbol " <<symbol->get_name() <<" from namespace " <<namespace_symbol->get_name();

            // Symbol is in the import name list so load it
            loadSymbol(symbol, namespace_symbols, file_scope);
          }
        }
      }
      else {
        mlog[TRACE] << "exists -> symbol " << symbol->get_name() << " from namespace " << namespace_symbol->get_name();
      }
    }
  }
}

void ModuleBuilder::insertSymbol(SgSymbol* symbol, SgGlobal* file_scope)
{
  ASSERT_not_null(symbol);

  SgName symbol_name = symbol->get_name();
  if (file_scope->symbol_exists(symbol_name) == false) {
    // The symbol doesn't exist in the file's scope so insert an alias for it
    SgAliasSymbol* alias_symbol = new SgAliasSymbol(symbol);
    ASSERT_not_null(alias_symbol);
    file_scope->insert_symbol(alias_symbol->get_name(), alias_symbol);
    mlog[TRACE] << "inserted symbol " << symbol->get_name();
  }
}

void ModuleBuilder::loadSymbol(SgSymbol* symbol, SgSymbolTable* symbol_table, SgGlobal* file_scope)
{
  ASSERT_not_null(symbol);

  if (file_scope->symbol_exists(symbol->get_name())) {
    return;
  }
  insertSymbol(symbol, file_scope);

  // Additional symbols may need to be loaded based on symbol type
  if (SgVariableSymbol* variable_symbol = isSgVariableSymbol(symbol)) {
    loadSymbol(variable_symbol, symbol_table, file_scope);
  }
  else if (SgClassSymbol* class_symbol = isSgClassSymbol(symbol)) {
    loadSymbol(class_symbol, symbol_table, file_scope);
  }
  else if (SgEnumSymbol* enum_symbol = isSgEnumSymbol(symbol)) {
    loadSymbol(enum_symbol, symbol_table, file_scope);
  }
}

void ModuleBuilder::loadSymbol(SgClassSymbol* symbol, SgSymbolTable* symbol_table, SgGlobal* file_scope)
{
  // Load class members
  SgClassDeclaration* decl = nullptr;

  // TODO: def_decl is nullptr so using table statement ...
  SgJovialTableStatement* defining_decl = nullptr;
  SgClassDefinition* table_def = nullptr;

  if (symbol) decl = symbol->get_declaration();
  if (decl) defining_decl = isSgJovialTableStatement(decl->get_definingDeclaration());
  if (defining_decl) table_def = defining_decl->get_definition();

#define TODO_DEF_DECL 0
#if TODO_DEF_DECL
  SgClassDefinition* def_decl = nullptr;
  if (decl) def_decl = isSgClassDefinition(decl->get_definingDeclaration());
#endif

  if (table_def) {
    for (SgDeclarationStatement* item_decl : table_def->get_members()) {
      if (SgVariableDeclaration* var_decl = isSgVariableDeclaration(item_decl)) {
        for (SgInitializedName* init_name : var_decl->get_variables()) {
          if (SgSymbol* var_symbol = init_name->search_for_symbol_from_symbol_table()) {
            loadSymbol(var_symbol, symbol_table, file_scope);
          }
        }
      }
    }
  }
}

void ModuleBuilder::loadSymbol(SgEnumSymbol* symbol, SgSymbolTable* symbol_table, SgGlobal* file_scope)
{
  // Load enumerators
  SgEnumDeclaration* decl = nullptr;
  if (symbol) decl = symbol->get_declaration();
  if (decl)   decl = isSgEnumDeclaration(decl->get_definingDeclaration());
  if (decl) {
    for (SgInitializedName* init_name : decl->get_enumerators()) {
      SgName symbol_name = init_name->get_name();
      if (SgSymbol* enum_symbol = init_name->search_for_symbol_from_symbol_table()) {
        loadSymbol(enum_symbol, symbol_table, file_scope);
      }
    }
  }
}

void ModuleBuilder::loadSymbol(SgVariableSymbol* symbol, SgSymbolTable* symbol_table, SgGlobal* file_scope)
{
  if (symbol) {
    if (SgInitializedName* init_name = symbol->get_declaration()) {
      if (SgType* type = init_name->get_typeptr()) {
        loadTypeSymbol(type, symbol_table, file_scope);
      }
    }
  }
}

void ModuleBuilder::loadTypeSymbol(SgType* type, SgSymbolTable* symbol_table, SgGlobal* file_scope)
{
  if (auto named_type = isSgNamedType(type)) {
    SgName type_name = named_type->get_name();

    // Ensure the symbol does not exist, otherwise could lead to an infinite recursion
    if (!file_scope->symbol_exists(type_name)) {
      if (SgClassSymbol* class_symbol = symbol_table->find_class(type_name)) {
        insertSymbol(class_symbol, file_scope);
        loadSymbol(class_symbol, symbol_table, file_scope);
      }
      else if (SgEnumSymbol* enum_symbol = symbol_table->find_enum(type_name)) {
        insertSymbol(enum_symbol, file_scope);
        loadSymbol(enum_symbol, symbol_table, file_scope);
      }
      else if (SgTypedefSymbol* typedef_symbol = symbol_table->find_typedef(type_name)) {
        insertSymbol(typedef_symbol, file_scope);
        if (SgTypedefDeclaration* typedef_decl = typedef_symbol->get_declaration()) {
          loadTypeSymbol(typedef_decl->get_base_type(), symbol_table, file_scope);
        }
      }
    }
  }
  else if (auto pointer_type = isSgPointerType(type)) {
    loadTypeSymbol(pointer_type->get_base_type(), symbol_table, file_scope);
  }
  else if (auto modifier_type = isSgModifierType(type)) {
    loadTypeSymbol(modifier_type->get_base_type(), symbol_table, file_scope);
  }
}

SgSourceFile* ModuleBuilder::getModule(const std::string &module_name)
{
  // A note about the syntax. The typename (or class) qualifier is required to give a hint to the
  // compiler because the iterator has a template parameter. Find returns a pair so second is used to
  // return the value.
  typename ModuleMapType::iterator mapIterator = moduleNameMap.find(module_name);
  SgSourceFile* module_file = (mapIterator != moduleNameMap.end()) ? mapIterator->second : nullptr;

  // No need to read the module file if file was already parsed
  if (module_file) {
    // Since the module file wasn't parsed the file scope won't be on the stack, so push it
    SgGlobal* file_scope = module_file->get_globalScope();
    ASSERT_require(file_scope->isCaseInsensitive());
    SageBuilder::pushScopeStack(file_scope);

    return module_file;
  }

  // Run the parser on the module file and load the declarations into global scope
  module_file = createSgSourceFile(module_name);

  if (module_file == nullptr) {
    mlog[ERROR] << "ModuleBuilder::getModule: No file found for the module: " << module_name;
    ROSE_ABORT();
  }
  else {
    // Store the parsed module file into the map (this is the only location where the map is modified)
    moduleNameMap.insert(ModuleMapType::value_type(module_name, module_file));
  }

  return module_file;
}

SgSourceFile* ModuleBuilder::createSgSourceFile(const std::string &module_name)
{
  int errorCode{0};
  std::string module_filename;
  std::vector<std::string> argv;

  // First look for a file in the current directory with a ".cpl" suffix; this seems to be the usage
  // pattern of users (though perhaps nonstandard). An added benefit is that it removes the need
  // to first parse the ".cpl" file, then generate and subsequently read a ".rcmp" file.
  fs::path path = fs::current_path() / (module_name + ".cpl");
  if (fs::exists(path)) {
    module_filename = path.string();
  }
  else {
    // Otherwise look for the ".rcmp" file in search paths
    std::string name = StringUtility::convertToLowerCase(module_name) + getModuleFileSuffix();
    module_filename = findFileFromInputDirs(name);
  }

  if (!fs::exists(module_filename)) {
    mlog[ERROR] << "Module file filename = " << module_filename << " NOT FOUND (expected to be present)";
    ROSE_ABORT();
  }

  argv.push_back(SKIP_SYNTAX_CHECK);
  argv.push_back(module_filename);

  nestedSgFile++;

  SgProject* project = getCurrentProject();
  ASSERT_not_null(project);

  SgSourceFile* newFile = isSgSourceFile(determineFileType(argv,errorCode,project));
  ASSERT_not_null(newFile);

  // Don't want to unparse or compile a module file in front-end
  newFile->set_skip_unparse(true);
  newFile->set_skipfinalCompileStep(true);

  // Run the frontend to process the compool file
  newFile->runFrontend(errorCode);

  if (errorCode != 0) {
    mlog[ERROR] << "In ModuleBuilder::createSgSourceFile(): frontend returned 0";
    ASSERT_require(errorCode == 0);
  }

  ASSERT_not_null (newFile);
  ASSERT_not_null (newFile->get_startOfConstruct());
  ASSERT_require  (newFile->get_parent() == project);
  project->set_file(*newFile);

  nestedSgFile--;

  return newFile;
}

void ModuleBuilder::dumpMap()
{
  std::map<std::string,SgSourceFile*>::iterator iter;

  std::cout << "Module file map::" << "\n";
  for (iter = moduleNameMap.begin(); iter != moduleNameMap.end(); iter++) {
    std::cout <<"FIRST : " << (*iter).first << " SECOND : " << (*iter).second << "\n";
  }
}

} // namespace Rose
