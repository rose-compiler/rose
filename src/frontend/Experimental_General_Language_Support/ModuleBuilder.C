#include "sage3basic.h"
#include "ModuleBuilder.h"

namespace Rose {

using namespace Rose::Diagnostics;

void ModuleBuilder::setCurrentProject(SgProject* project)
{
  currentProject = project;
}

std::string ModuleBuilder::find_file_from_inputDirs(const std::string &basename)
 {
   std::string dir;
   std::string name;
   int sizeArg = inputDirs.size();

   for (int i = 0; i< sizeArg; i++) {
     dir = inputDirs[i];
     name = dir+"/"+ basename;

     std::string tmp = name + getModuleFileSuffix();
     if (boost::filesystem::exists(tmp)) {
       return name;
     }
   }
   return basename;
}

void ModuleBuilder::setInputDirs(SgProject* project)
{
  std::vector<std::string> args = project->get_originalCommandLineArgumentList();
  std::string  rmodDir;

  // Add path to iso_c_binding.rmod. The intrinsic modules have been placed in the
  // 3rdPartyLibraries because they could be compiler dependent. If placed there we could
  // reasonable have multiple versions at some point.
  //
  // WARNING - this is for Fortran but really not needed for Jovial as not intrinsic compool modules
  //
  std::string intrinsic_mod_path = findRoseSupportPathFromSource("src/3rdPartyLibraries/fortran-parser", "share/rose");
  inputDirs.push_back(intrinsic_mod_path);

  int sizeArgs = args.size();

  for (int i = 0; i< sizeArgs; i++) {
    if (args[i].find("-I",0)==0) {
      rmodDir = args[i].substr(2);
      std::string rmodDir_no_quotes = boost::replace_all_copy(rmodDir, "\"", "");

      if (boost::filesystem::exists(rmodDir_no_quotes.c_str())) {
        inputDirs.push_back(rmodDir_no_quotes);
      }
      else {
        if (Rose::ir_node_mlog[Rose::Diagnostics::DEBUG]) {
          std::cout << "WARNING: the input directory does not exist (rose): " << rmodDir << std::endl;
        }
      }
    }
  }
}

void ModuleBuilder::loadModule(const std::string &module_name, SgGlobal* file_scope)
{
  SgNamespaceDeclarationStatement* namespace_decl = nullptr;
  SgNamespaceDefinitionStatement*  namespace_defn = nullptr;
  SgNamespaceSymbol* namespace_symbol = nullptr;
  SgSymbolTable* namespace_symbols = nullptr;

  SgSourceFile* source = getModule(module_name);
  ROSE_ASSERT(source);

  // A loaded module doesn't need to be unparsed or compiled in the back-end
  source->set_skip_unparse(true);
  source->set_skipfinalCompileStep(true);

  SgGlobal* module_scope = source->get_globalScope();
  ROSE_ASSERT(module_scope);

  namespace_symbol = module_scope->lookup_namespace_symbol(module_name);
  if (namespace_symbol) namespace_decl = namespace_symbol->get_declaration();
  if (namespace_decl) namespace_defn = namespace_decl->get_definition();
  if (namespace_defn) namespace_symbols = namespace_defn->get_symbol_table();

  // inject namespace symbols into the file_scope of the caller
  if (namespace_symbols) {
#if 0
    std::cout << "--> inserting symbols from found namespace symbol  " << namespace_symbol->get_name() << " " << namespace_symbol << std::endl;
#endif

    BOOST_FOREACH(SgNode* node, namespace_symbols->get_symbols()) {
      SgSymbol* symbol = isSgSymbol(node);
      ROSE_ASSERT(symbol);

      if (file_scope->symbol_exists(symbol->get_name()) == false) {
#if 0
        std::cout << "    : inserting symbol " << symbol->get_name()
                  << " from namespace " << namespace_symbol->get_name() << std::endl;
#endif
        SgAliasSymbol* alias_symbol = new SgAliasSymbol(symbol);
        ROSE_ASSERT(alias_symbol);
        file_scope->insert_symbol(alias_symbol->get_name(), alias_symbol);
      }
    }
  }
}

SgSourceFile* ModuleBuilder::getModule(const std::string &module_name)
{
  // A note about the syntax. The typename (or class) qualifier is required to give a hint to the
  // compiler because the iterator has a template parameter. Find returns a pair so second is used to
  // return the value.
  typename ModuleMapType::iterator mapIterator = moduleNameMap.find(module_name);
  SgSourceFile* module_file = (mapIterator != moduleNameMap.end()) ? mapIterator->second : NULL;

#if 0
  std::cout << "--> searching for module: " << module_name << std::endl;
#endif

  // No need to read the module file if file was already parsed
  if (module_file) {
    // Since the module file wasn't parsed the file scope won't be on the stack, so push it
    SgGlobal* file_scope = module_file->get_globalScope();
    SageBuilder::pushScopeStack(file_scope);
#if 0
    std::cout << "    found existing module in map: " << module_name << std::endl;
    std::cout << "    file scope is : " << file_scope << std::endl;
#endif
    return module_file;
  }

  std::string lc_module_name = StringUtility::convertToLowerCase(module_name);
  std::string file_path = find_file_from_inputDirs(lc_module_name);

  // This will run the parser on the module file and load the declarations into global scope
  module_file = createSgSourceFile(file_path);

  if (module_file == nullptr) {
    mlog[ERROR] << "ModuleBuilder::getModule: No file found for the module file: "
                << lc_module_name << std::endl;
    ROSE_ASSERT(false);
  }
  else {
#if 0
    std::cout << "    storing module in map: " << module_name << std::endl;
#endif

    // Store the parsed module file into the map (this is the only location where the map is modified)
    moduleNameMap.insert(ModuleMapType::value_type(module_name, module_file));
  }

  return module_file;
}

SgSourceFile* ModuleBuilder::createSgSourceFile(const std::string &module_name)
{
  int errorCode = 0;
  std::vector<std::string> argv;

  // current directory
  std::string module_filename = boost::algorithm::to_lower_copy(module_name) + getModuleFileSuffix();

  if (boost::filesystem::exists(module_filename) == false) {
    mlog[ERROR] << "Module file filename = " << module_filename << " NOT FOUND (expected to be present) \n";
    ROSE_ASSERT(false);
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
    mlog[ERROR] << "In ModuleBuilder<T>::createSgSourceFile(): frontend returned 0 \n";
    ROSE_ASSERT(errorCode == 0);
  }

  ASSERT_not_null (newFile);
  ASSERT_not_null (newFile->get_startOfConstruct());
  ROSE_ASSERT (newFile->get_parent() == project);
  project->set_file(*newFile);

  nestedSgFile--;

  return newFile;
}

void ModuleBuilder::dumpMap()
{
  std::map<std::string,SgSourceFile*>::iterator iter;

  std::cout << "Module file map::" << std::endl;
  for (iter = moduleNameMap.begin(); iter != moduleNameMap.end(); iter++) {
    std::cout <<"FIRST : " << (*iter).first << " SECOND : " << (*iter).second << std::endl;
  }
}

} // namespace Rose
