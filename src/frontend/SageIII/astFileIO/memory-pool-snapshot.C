
#include "sage3basic.h"
#include "memory-pool-snapshot.h"

namespace Rose {

namespace AST {

  namespace utils {
    template <typename ...Ts>
    struct NodeList;

    template <typename N, typename ...Ns>
    struct NodeList<N, Ns...> {
      template <typename Functor>
      static inline void apply() {
        NodeList<N>::template apply<Functor>();
        NodeList<Ns...>::template apply<Functor>();
      }
      template <typename Functor>
      static inline void apply(Functor & f) {
        NodeList<N>::template apply<Functor>(f);
        NodeList<Ns...>::template apply<Functor>(f);
      }
    };

    template <typename Node>
    struct NodeList<Node> {
      template <typename Functor>
      static inline void apply() {
        Functor::template apply<Node>();
      }

      template <typename Functor>
      static inline void apply(Functor & f) {
        f.template apply<Node>();
      }
    };
  }
}

namespace MemPool {

struct CSVDump {
  std::ostream & out;
  
  CSVDump(std::ostream & out_, std::vector<std::string> const & cols) :
    out(out_)
  {
    std::copy(cols.begin(), cols.end(), std::ostream_iterator<std::string>(out, ","));
    out.seekp(-1, out.cur);
    out << std::endl;
  }
};

struct DumpPoolInfo : CSVDump {
  static const std::vector<std::string> header;
  
  DumpPoolInfo(std::ostream & out_) :
    CSVDump(out_, header)
  {}

  template <typename N>
  void apply() {
//  std::cout << N::static_variant << " " << N::pools.size() << " " << N::pool_size << std::endl;
    N * n0 = N::pools.size() > 0 ? &(((N*)N::pools[0])[0]) : nullptr;
    n0 = ( n0 && n0->get_freepointer() == AST_FileIO::IS_VALID_POINTER() ) ? n0 : nullptr;
    for (size_t p = 0; p < N::pools.size(); p++) {
      for (size_t o = 0; o < N::pool_size; o++) {
        N * n = &(((N*)N::pools[p])[o]);
        void * freepointer = n->get_freepointer();
        out << std::dec << N::static_variant;

        out << "," << std::dec << p << "," << std::dec << o << "," << std::hex << n << "," << std::hex << freepointer << ",";

        if ( freepointer == AST_FileIO::IS_VALID_POINTER()) {
          out << std::hex << n->get_parent();
        } else {
          out << "0";
        }
          
        out << std::endl;
      }
    }
  }
};
const std::vector<std::string> DumpPoolInfo::header{ "kind", "pool", "offset", "freepointer" };

using Nodes = AST::utils::NodeList<SgAliasSymbol,SgDirectoryList,Sg_File_Info,SgFileList,SgFunctionTypeSymbol,SgFunctionTypeTable,SgGlobal,SgInitializedName,SgProject,SgSourceFile,SgStorageModifier,SgSupport,SgSymbolTable,SgTypeDefault,SgTypedefSeq,SgTypeInt,SgTypeTable,SgVariableDeclaration,SgVariableDefinition,SgVariableSymbol>;
//using Nodes = AST::utils::NodeList<SgGlobal, SgSymbolTable, SgTypeTable, Sg_File_Info>;

void snapshot(std::ostream & out) {
  DumpPoolInfo f(out);
  Nodes::apply(f);
}

void snapshot(std::string const & outfilename) {
  std::ofstream ofile(outfilename);
  snapshot(ofile);
}

}
}
