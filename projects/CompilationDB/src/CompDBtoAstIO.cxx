
#include "sage3basic.h"
#include "AST_FILE_IO.h"

#include "nlohmann/json.hpp"

#include <cassert>

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

      protected:
        json db;

        SgProject * project;

        std::vector<state_t> states;
        std::vector<boost::filesystem::path> directories;
        std::vector<boost::filesystem::path> filenames;
        std::vector<std::vector<std::string>> arguments;
        std::vector<SgSourceFile *> sourcefiles;
        std::vector<int> backend_status;

        boost::filesystem::path cwd;

      protected:
        void frontend();

        void init(const char * cdbfn, const char * bfn);

      public:
        CompilationDB(const char * cdbfn, const char * bfn);
        virtual ~CompilationDB();

        void unparse();
        void compile();
    };
  }
}

namespace ROSE { namespace SageInterface {

CompilationDB::CompilationDB(const char * cdbfn, const char * bfn) :
  db(),
  project(NULL),
  states(),
  directories(),
  filenames(),
  sourcefiles(),
  backend_status(),
  cwd(boost::filesystem::current_path())
{
  init(cdbfn, bfn);
}

CompilationDB::~CompilationDB() {
  if (project != NULL)
    delete project;
}

void CompilationDB::init(const char * cdbfn, const char * bfn) {
  // Read JSON compilation database
  std::ifstream cdbf(cdbfn);
  cdbf >> db;

  // Iterate over the compilation units
  for (auto& desc : db) {
    states.push_back(e_init);
    directories.push_back(desc["directory"].get<std::string>());
    filenames.push_back(desc["file"].get<std::string>());

    arguments.push_back(std::vector<std::string>());
    std::vector<std::string> & args = arguments.back();
    for (auto& e: desc["arguments"]) {
      args.push_back(e.get<std::string>());
    }

    sourcefiles.push_back(NULL);
    backend_status.push_back(-1);
  }

  // Either load the AST from binary or calls the frontend (then write the AST)
  boost::filesystem::path bf(bfn);
  if (boost::filesystem::exists(bf)) {
    project = (SgProject*) AST_FILE_IO::readASTFromFile(bf.string());
    assert(project != NULL);

    SgFilePtrList & fl = project->get_fileList();
    assert(fl.size() == states.size());

    for (size_t i = 0; i < states.size(); i++) {
      sourcefiles[i] = isSgSourceFile(fl[i]);
      assert(sourcefiles[i] != NULL);

      // TODO check match with filenames and arguments?

      states[i] = e_loaded;
    }
  } else {
    project = new SgProject();

    for (size_t i = 0; i < states.size(); i++) {
      boost::filesystem::current_path(directories[i]);

      project->set_originalCommandLineArgumentList(arguments[i]);

      sourcefiles[i] = isSgSourceFile(SageBuilder::buildFile(filenames[i].string(), SgName(), project));
      ROSE_ASSERT(sourcefiles[i] != NULL);

      states[i] = e_loaded;
    }
    boost::filesystem::current_path(cwd);

    AST_FILE_IO::startUp(project);
    AST_FILE_IO::writeASTToFile(bf.string());
  }
}

void CompilationDB::frontend() {
  for (size_t i = 0; i < states.size(); i++) {
    assert(states[i] == e_init);

    boost::filesystem::current_path(directories[i]);

    project->set_originalCommandLineArgumentList(arguments[i]);

    sourcefiles[i] = isSgSourceFile(SageBuilder::buildFile(filenames[i].string(), SgName(), project));
    ROSE_ASSERT(sourcefiles[i] != NULL);

    states[i] = e_loaded;
  }

  boost::filesystem::current_path(cwd);
}

void CompilationDB::unparse() {
  for (size_t i = 0; i < states.size(); i++) {
    assert(states[i] == e_loaded);

    boost::filesystem::current_path(directories[i]);

    sourcefiles[i]->unparse();

    states[i]  = e_unparsed;
  }

  boost::filesystem::current_path(cwd);
}

void CompilationDB::compile() {
  for (size_t i = 0; i < states.size(); i++) {
    assert(states[i] == e_unparsed);

    boost::filesystem::current_path(directories[i]);

    backend_status[i] = sourcefiles[i]->compileOutput(0);

    states[i] = ( backend_status[i] == 0 ) ? e_compiled : e_failed;
  }

  boost::filesystem::current_path(cwd);
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
    cdb.compile();
  }

  return 0;
}


