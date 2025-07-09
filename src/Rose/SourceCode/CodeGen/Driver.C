
#include "sage3basic.h"

#include "Rose/SourceCode/CodeGen/Driver.h"

#ifndef VERBOSE
# define VERBOSE 0
#endif

namespace Rose { namespace SourceCode { namespace CodeGen {

bool ignore(const std::string & name) {
  return name == "" || name.find("__builtin") == 0 || name.find("__frontend_specific_variable_to_provide_header_file_path") == 0;
}

bool ignore(SgScopeStatement * scope) {
  return isSgBasicBlock(scope);
}

#if 0 // [Robb Matzke 2022-11-16]: unused
static boost::filesystem::path resolve(
    const boost::filesystem::path & p,
    const boost::filesystem::path & base = boost::filesystem::current_path()
) {
    boost::filesystem::path abs_p = p;
    boost::filesystem::path result;
    for(auto it = abs_p.begin(); it != abs_p.end(); ++it) {
      if(*it == "..") {
        if (boost::filesystem::is_symlink(result) ) {
          // /a/b/.. is not necessarily /a if b is a symbolic link
          result /= *it;
        } else if (result.filename() == "..") {
          // /a/b/../.. is not /a/b/.. under most circumstances
          // We can end up with ..s in our result because of symbolic links
          result /= *it;
        } else {
          // Otherwise it should be safe to resolve the parent
          result = result.parent_path();
        }
      } else if(*it == ".") {
        // Ignore
      } else {
        // Just cat other path entries
        result /= *it;
      }
    }
    return result;
}
#endif

size_t Driver::getFileID(const boost::filesystem::path & path) const {
  auto it_file_id = path_to_id_map.find(path);
  assert(it_file_id != path_to_id_map.end());
  return it_file_id->second;
}

size_t Driver::getFileID(SgSourceFile * source_file) const {
  auto it_file_id = file_to_id_map.find(source_file);
  assert(it_file_id != file_to_id_map.end());
  return it_file_id->second;
}

size_t Driver::getFileID(SgScopeStatement * scope) const {
  SgFile * enclosing_file = SageInterface::getEnclosingFileNode(scope);
  assert(enclosing_file != NULL); // FIXME Contingency : Scope Stack

  SgSourceFile * enclosing_source_file = isSgSourceFile(enclosing_file);
  assert(enclosing_source_file != NULL);

  return getFileID(enclosing_source_file);
}

void Driver::init(SgProject* /*project_*/) {
  ASSERT_not_null(project);

  if (!CommandlineProcessing::isCppFileNameSuffix("hpp"))
    CommandlineProcessing::extraCppSourceFileSuffixes.push_back("hpp");

  if (!CommandlineProcessing::isCppFileNameSuffix("h"))
    CommandlineProcessing::extraCppSourceFileSuffixes.push_back("h");

  // Load existing files
  auto const & files = project->get_fileList_ptr()->get_listOfFiles();
  for (auto it_file = files.begin(); it_file != files.end(); it_file++) {
    SgSourceFile * src_file = isSgSourceFile(*it_file);
    if (src_file != NULL) {
      add(src_file);
      //! \todo get path and add to path_to_id_map
    }
  }
}

Driver::Driver() :
  project(new SgProject()),
  file_id_counter(1), // 0 is reserved
  path_to_id_map(),
  id_to_file_map(),
  file_to_id_map(),
  file_id_to_accessible_file_id_map()
{ 
  init(project);
}

Driver::Driver(SgProject * project_) :
  project(project_),
  file_id_counter(1), // 0 is reserved
  path_to_id_map(),
  id_to_file_map(),
  file_to_id_map(),
  file_id_to_accessible_file_id_map()
{ 
  init(project);
}

Driver::Driver(std::vector<std::string> & args) :
  project(args.size() > 1 ? ::frontend(args) : new SgProject()),
  file_id_counter(1), // 0 is reserved
  path_to_id_map(),
  id_to_file_map(),
  file_to_id_map(),
  file_id_to_accessible_file_id_map()
{ 
  init(project);
}

size_t Driver::add(SgSourceFile * file) {
  size_t id = file_id_counter++;

  id_to_file_map.insert(std::pair<size_t, SgSourceFile *>(id, file));
  file_to_id_map.insert(std::pair<SgSourceFile *, size_t>(file, id));
  file_id_to_accessible_file_id_map.insert(std::pair<size_t, std::set<size_t> >(id, std::set<size_t>())).first->second.insert(id);

  //! \todo detect other accessible file (included) (opt: add recursively)

  return id;
}

size_t Driver::add(const boost::filesystem::path & path) {
  if (!boost::filesystem::exists(path) || !boost::filesystem::is_regular_file(path)) {
    std::cerr << "[Error] (Driver::add) Cannot find: " << path << std::endl;
    ROSE_ABORT();
  }

  auto it_file_id = path_to_id_map.find(path);
  if (it_file_id != path_to_id_map.end()) {
    return it_file_id->second;
  } else {
    SgSourceFile * file = isSgSourceFile(SageBuilder::buildFile(path.string(), path.string(), project));
    assert(file != NULL);
    file->set_skip_unparse(true);
    file->set_skipfinalCompileStep(true);

    size_t id = add(file);
    path_to_id_map.insert(std::pair<boost::filesystem::path, size_t>(path, id));
    return id;
  }
}

void Driver::addCxxExtension(std::string suffix) const {
  if (!CommandlineProcessing::isCppFileNameSuffix(suffix)) {
    CommandlineProcessing::extraCppSourceFileSuffixes.push_back(suffix);
  }
}

size_t Driver::create(const boost::filesystem::path & path) {
  assert(path_to_id_map.find(path) == path_to_id_map.end());

  if (boost::filesystem::exists(path))
    boost::filesystem::remove(path);

  auto it_file_id = path_to_id_map.find(path);
  assert(it_file_id == path_to_id_map.end());
    
  SgSourceFile * file = isSgSourceFile(SageBuilder::buildFile(path.string(), path.string(), project));
  assert(file != NULL);
//SageInterface::attachComment(file, "/* Generated by ROSE Compiler */");
  file->set_skipfinalCompileStep(true);

  size_t id = file_id_counter++;

  id_to_file_map.insert(std::pair<size_t, SgSourceFile *>(id, file));
  file_to_id_map.insert(std::pair<SgSourceFile *, size_t>(file, id));
  path_to_id_map.insert(std::pair<boost::filesystem::path, size_t>(path, id));
  file_id_to_accessible_file_id_map.insert(std::pair<size_t, std::set<size_t> >(id, std::set<size_t>())).first->second.insert(id);

  return id;
}

void Driver::setUnparsedFile(size_t file_id) const {
  auto it_file = id_to_file_map.find(file_id);
  assert(it_file != id_to_file_map.end());
  it_file->second->set_skip_unparse(false);
}

void Driver::setCompiledFile(size_t file_id) const {
  auto it_file = id_to_file_map.find(file_id);
  assert(it_file != id_to_file_map.end());
  it_file->second->set_skipfinalCompileStep(false);
}

void Driver::addExternalHeader(size_t file_id, std::string header_name, bool is_system_header) const {
  auto it_file = id_to_file_map.find(file_id);
  assert(it_file != id_to_file_map.end());
  
  SgSourceFile * file = it_file->second;
  assert(file != NULL);

  SageInterface::insertHeader(file, header_name, is_system_header);
}

void Driver::addPragmaDecl(size_t file_id, std::string str) const {
  auto it_file = id_to_file_map.find(file_id);
  assert(it_file != id_to_file_map.end());
  
  SgSourceFile * file = it_file->second;
  assert(file != NULL);
  SgGlobal * global = file->get_globalScope();

  SageInterface::prependStatement(SageBuilder::buildPragmaDeclaration(str, global), global);
}

SgSourceFile * Driver::getSourceFile(size_t id) const {
  auto it = id_to_file_map.find(id);
  return it != id_to_file_map.end() ? it->second : NULL;  
}

SgGlobal * Driver::getGlobalScope(size_t id) const {
  auto it = id_to_file_map.find(id);
  return it != id_to_file_map.end() ? (it->second != NULL ? it->second->get_globalScope() : NULL) : NULL;  
}

} } }
