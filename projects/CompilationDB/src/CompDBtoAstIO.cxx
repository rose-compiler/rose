
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

      public:
        CompilationDB(const char * cdbfn);
        virtual ~CompilationDB();

        void frontend();
        void unparse();
        void compile();
        
        void write(const char * obfn);
    };
  }
}

namespace ROSE { namespace SageInterface {

CompilationDB::CompilationDB(const char * cdbfn) :
  db(),
  project(new SgProject()),
  states(),
  directories(),
  filenames(),
  sourcefiles(),
  backend_status(),
  cwd(boost::filesystem::current_path())
{
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
}

CompilationDB::~CompilationDB() {
  delete project;
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

void CompilationDB::write(const char * obfn) {
  AST_FILE_IO::startUp(project);
  AST_FILE_IO::writeASTToFile(obfn);
}

}}

int main(int argc, char ** argv) {
  assert(argc >= 2);

  ROSE::SageInterface::CompilationDB cdb(argv[1]);

  cdb.frontend();
  cdb.unparse();
  cdb.compile();

  if (argc > 2) {
    cdb.write(argv[2]);
  }

  return 0;
}


