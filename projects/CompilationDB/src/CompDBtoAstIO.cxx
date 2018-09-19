
#include "sage3basic.h"

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
          e_compiled
        } state_t;

      protected:
        state_t state;

        json db;

        SgProject * project;

        std::vector<std::pair<json, SgSourceFile *> > files;

        boost::filesystem::path cwd;

      public:
        CompilationDB(const char * cdbfn);
        virtual ~CompilationDB();

        void build();
        void unparse();
        void compile();
    };
  }
}

namespace ROSE { namespace SageInterface {

CompilationDB::CompilationDB(const char * cdbfn) :
  state(e_init),
  db(),
  project(new SgProject()),
  files(),
  cwd(boost::filesystem::current_path())
{
  std::ifstream cdbf(cdbfn);
  cdbf >> db;
}

CompilationDB::~CompilationDB() {
  delete project;
}

void CompilationDB::build() {
  // Check current state
  ROSE_ASSERT(state == e_init);

  // Iterate over the compilation units
  for (auto& cu : db) {

    // Source file associated with this compilation unit
    std::string filename = cu["file"].get<std::string>();

    // Loading the command line
    std::vector<std::string> args;
    for (auto& e: cu["arguments"]) {
      args.push_back(e.get<std::string>());
    }

    project->set_originalCommandLineArgumentList(args);

    // Set working directory (in case the command line contains relative path)
    boost::filesystem::current_path(cu["directory"].get<std::string>());

    // Calls the frontend
    SgFile * file = SageBuilder::buildFile(filename, SgName(), project);
    ROSE_ASSERT(file != NULL);

    // Check: frontend must produce a source file
    SgSourceFile * src_file = isSgSourceFile(file);
    ROSE_ASSERT(src_file != NULL);

    files.push_back(std::pair<json, SgSourceFile *>(cu, src_file));
  }

  // Restore starting working directory
  boost::filesystem::current_path(cwd);

  // Set new state
  state = e_loaded;
}

void CompilationDB::unparse() {
  // Check current state
  ROSE_ASSERT(state == e_loaded);

  // Iterate over the compilation units
  for (auto& f : files) {
    json cu = f.first;
    SgSourceFile * file = f.second;

    // TODO
  }

  // Restore starting working directory
  boost::filesystem::current_path(cwd);

  // Set new state
  state = e_unparsed;
}

void CompilationDB::compile() {
  // Check current state
  ROSE_ASSERT(state == e_unparsed);

  // Iterate over the compilation units
  for (auto& f : files) {
    json cu = f.first;
    SgSourceFile * file = f.second;

    // TODO
  }

  // Restore starting working directory
  boost::filesystem::current_path(cwd);

  // Set new state
  state = e_compiled;
}

}}

int main(int argc, char ** argv) {
  assert(argc >= 2);

  ROSE::SageInterface::CompilationDB cdb(argv[1]);

  cdb.build();
  cdb.unparse();
  cdb.compile();

  return 0;
}


