
#include "sage3basic.h"
#include "AST_FILE_IO.h"

#include "nlohmann/json.hpp"

#include <cassert>

#define BUGFIX_FOR_ROSE_1385 1

namespace ROSE {
  using nlohmann::json;

  namespace SageInterface {
    class CompilationDB {
      public:
        typedef enum {
          e_init,
          e_loaded,
          e_unparsed,
          e_compiled,
          e_failed
        } state_t;

        struct file_t {
          state_t state;
          boost::filesystem::path directory;
          boost::filesystem::path path;
          std::vector<std::string> arguments;
          SgSourceFile * file;
          int backend_status;

          file_t(const json & desc);
        };

      protected:
        json db;

        SgProject * project;

        std::vector<file_t> files;

        boost::filesystem::path cwd;

      protected:
        void init(const char * cdbfn, const char * bfn);

      public:
        CompilationDB(const char * cdbfn, const char * bfn);
        virtual ~CompilationDB();

        void unparse(size_t i);
        void unparse();
        void backend(size_t i);
        void backend();
    };
  }
}

namespace ROSE { namespace SageInterface {

CompilationDB::file_t::file_t(const json & desc) :
  state(e_init),
  directory(desc["directory"].get<std::string>()),
  path(desc["file"].get<std::string>()),
  arguments(),
  file(NULL),
  backend_status(-1)
{
  for (auto& e: desc["arguments"]) {
#if BUGFIX_FOR_ROSE_1385
    std::string arg = e.get<std::string>();
    if (arg == "-std=c++11") {
      arguments.push_back("-std=c++14");
    } else {
      arguments.push_back(arg);
    }
#else
    arguments.push_back(e.get<std::string>());
#endif
  }
}

CompilationDB::CompilationDB(const char * cdbfn, const char * bfn) :
  db(),
  project(NULL),
  files(),
  cwd(boost::filesystem::current_path())
{
  init(cdbfn, bfn);
}

CompilationDB::~CompilationDB() {
  if (project != NULL)
    delete project;
}

void CompilationDB::init(const char * cdbfn, const char * bfn) {
  std::cout << "ENTER CompilationDB::init(...)" << std::endl;
  std::cout << "  -- Compilation DB = " << cdbfn << std::endl;
  std::cout << "  -- ROSE AST File  = " << bfn << std::endl;

  // Read JSON compilation database
  std::ifstream cdbf(cdbfn);
  cdbf >> db;

  // Iterate over the compilation units
  for (auto& desc : db) {
    files.emplace_back(desc);
  }

  // Either load the AST from binary or calls the frontend (then write the AST)
  boost::filesystem::path bf(bfn);
  if (!boost::filesystem::exists(bf)) {
    std::cout << " > Build ROSE AST" << std::endl;

    project = new SgProject();

    for (size_t i = 0; i < files.size(); i++) {
      std::cout << " > file[" << i << "] = " << files[i].path.string() << std::endl;
      for (size_t j = 0; j < files[i].arguments.size(); j++)
        std::cout << "   -- argv[" << j << "] = " << files[i].arguments[j] << std::endl;

      boost::filesystem::current_path(files[i].directory);

      project->set_originalCommandLineArgumentList(files[i].arguments);

      files[i].file = isSgSourceFile(SageBuilder::buildFile(files[i].path.string(), SgName(), project));
      ROSE_ASSERT(files[i].file != NULL);

      files[i].state = e_loaded;
    }
    boost::filesystem::current_path(cwd);

    AST_FILE_IO::startUp(project);
    AST_FILE_IO::writeASTToFile(bf.string());
  } else {
    std::cout << " > Loading saved ROSE AST..." << std::endl;

    project = (SgProject*) AST_FILE_IO::readASTFromFile(bf.string());
    assert(project != NULL);

    SgFilePtrList & fl = project->get_fileList();
    assert(fl.size() == files.size());

    for (size_t i = 0; i < files.size(); i++) {
      files[i].file = isSgSourceFile(fl[i]);
      assert(files[i].file != NULL);

      // TODO check match with filenames and arguments?

      files[i].state = e_loaded;
    }
  }

  std::cout << "LEAVE CompilationDB::init(...)" << std::endl;
}

void CompilationDB::unparse(size_t i) {
  std::cout << "ENTER CompilationDB::unparse(" << i << ")" << std::endl;

  assert(files[i].state == e_loaded);
  boost::filesystem::path owd = boost::filesystem::current_path();

  boost::filesystem::current_path(files[i].directory);

  project->set_originalCommandLineArgumentList(files[i].arguments);

  files[i].file->unparse();

  files[i].state = e_unparsed;

  boost::filesystem::current_path(owd);

  std::cout << "LEAVE CompilationDB::unparse(" << i << ")" << std::endl;
}

void CompilationDB::unparse() {
  for (size_t i = 0; i < files.size(); i++) {
    unparse(i);
  }
}

void CompilationDB::backend(size_t i) {
  std::cout << "ENTER CompilationDB::backend(" << i << ")" << std::endl;

  assert(files[i].state == e_unparsed);
  boost::filesystem::path owd = boost::filesystem::current_path();

  boost::filesystem::current_path(files[i].directory);

  project->set_originalCommandLineArgumentList(files[i].arguments);

  files[i].backend_status = files[i].file->compileOutput(0);

  files[i].state = ( files[i].backend_status == 0 ) ? e_compiled : e_failed;

  boost::filesystem::current_path(owd);

  std::cout << "LEAVE CompilationDB::backend(" << i << ")" << std::endl;
}

void CompilationDB::backend() {
  for (size_t i = 0; i < files.size(); i++) {
    backend(i);
  }
}

}}

int main(int argc, char ** argv) {
  assert(argc >= 3);

  ROSE::SageInterface::CompilationDB cdb(argv[1], argv[2]);

  std::vector<std::string> options(argv+3, argv+argc);

  if (std::find(options.begin(), options.end(), "unparse") != options.end()) {
    cdb.unparse();
  }

  if (std::find(options.begin(), options.end(), "compile") != options.end()) {
    cdb.backend();
  }

  return 0;
}


