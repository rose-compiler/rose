
#ifndef Rose_Source_Generation_API_txx
#define Rose_Source_Generation_API_txx

#include "sage3basic.h"

#include <iostream>

#include <Rose/Source/Generation/SymbolScanner.h>

namespace Rose { namespace Source { namespace Generation {

template <typename CRT>
void API<CRT>::load(Driver & driver) {
  load_headers(driver);
  load_api(driver);
}

template <typename CRT>
void API<CRT>::add_nodes_for_namequal(Driver & driver, SgSourceFile * srcfile) const {
  auto & extra_nodes_for_namequal_init = srcfile->get_extra_nodes_for_namequal_init();
  for (auto fid: file_ids) {
    extra_nodes_for_namequal_init.push_back(driver.getGlobalScope(fid));
  }
}

template <typename CRT>
void API<CRT>::display(std::ostream & out) const {
  out << "API of " << name << " :" << std::endl;
  out << "  Namespaces:" << std::endl;
  for (auto p: namespaces) {
    if (((CRT*)this)->*(p.second)) {
      out << "    " << p.first << " = (" << (((CRT*)this)->*(p.second))->class_name() << " *)" << std::hex << ((CRT*)this)->*(p.second) << std::endl;
    } else {
      out << "    " << p.first << " = Not Found" << std::endl;
    }
  }

  out << "  Classes:" << std::endl;
  for (auto p: classes) {
    if (((CRT*)this)->*(p.second)) {
      out << "    " << p.first << " = (" << (((CRT*)this)->*(p.second))->class_name() << " *)" << std::hex << ((CRT*)this)->*(p.second) << std::endl;
    } else {
      out << "    " << p.first << " = Not Found" << std::endl;
    }
  }

  out << "  Typedefs:" << std::endl;
  for (auto p: typedefs) {
    if (((CRT*)this)->*(p.second)) {
      out << "    " << p.first << " = (" << (((CRT*)this)->*(p.second))->class_name() << " *)" << std::hex << ((CRT*)this)->*(p.second) << std::endl;
    } else {
      out << "    " << p.first << " = Not Found" << std::endl;
    }
  }

  out << "  Variables:" << std::endl;
  for (auto p: variables) {
    if (((CRT*)this)->*(p.second)) {
      out << "    " << p.first << " = (" << (((CRT*)this)->*(p.second))->class_name() << " *)" << std::hex << ((CRT*)this)->*(p.second) << std::endl;
    } else {
      out << "    " << p.first << " = Not Found" << std::endl;
    }
  }

  out << "  Functions:" << std::endl;
  for (auto p: functions) {
    if (((CRT*)this)->*(p.second)) {
      out << "    " << p.first << " = (" << (((CRT*)this)->*(p.second))->class_name() << " *)" << std::hex << ((CRT*)this)->*(p.second) << std::endl;
    } else {
      out << "    " << p.first << " = Not Found" << std::endl;
    }
  }
}

template <typename CRT>
void API<CRT>::set_command_line(Driver & driver) const {
  std::vector<std::string> cmdline{"rose-codegen", "-c"};
  for (auto path: paths) {
    if (!boost::filesystem::exists( path )) {
      std::cerr << "[WARN] Path to API header files does not exist: " << path << std::endl;
    } else {
      cmdline.push_back("-I" + path);
    }
  }

  for (auto flag: flags) {
    cmdline.push_back(flag);
  }
  driver.project->set_originalCommandLineArgumentList(cmdline);
}

#define DEBUG_API__load_headers 0

template <typename CRT>
void API<CRT>::load_headers(Driver & driver) {
#if DEBUG_API__load_headers
  std::cout << "API<CRT>::load_headers" << std::endl;
#endif
  set_command_line(driver);
  driver.addCxxExtension("hxx");

  for (auto file: files) {
#if DEBUG_API__load_headers
    std::cout << "  file = " << file << std::endl;
#endif

    bool found = false;
    if (!cache.empty()) {
      // TODO Try to get AST file from cache directory else build AST file
      // TODO Need "driver.readAST(fp);" which could return a set of file_id...
    }

    if (!found) {
      for (auto path: paths) {
        auto fp = path + "/" + file;
        if (boost::filesystem::exists(fp)) {
          file_ids.insert(driver.add(fp));
          found = true;
          break;
        }
      }
    }

    if (!found) {
      std::cerr << "[WARN] API header file not found: " << file << std::endl;
    }
  }

  ROSE_ASSERT(SageBuilder::topScopeStack() == NULL); // Sanity check
}

template <typename CRT>
void API<CRT>::load_api(Driver & /*driver*/) {
  SymbolScanner<CRT> scanner(*(CRT*)this);

  SgNamespaceSymbol::traverseMemoryPoolNodes(scanner);

  SgClassSymbol::traverseMemoryPoolNodes(scanner);
  SgTemplateClassSymbol::traverseMemoryPoolNodes(scanner);

  SgTypedefSymbol::traverseMemoryPoolNodes(scanner);
  SgTemplateTypedefSymbol::traverseMemoryPoolNodes(scanner);

  SgVariableSymbol::traverseMemoryPoolNodes(scanner);
  SgTemplateVariableSymbol::traverseMemoryPoolNodes(scanner);

  SgFunctionSymbol::traverseMemoryPoolNodes(scanner);
  SgMemberFunctionSymbol::traverseMemoryPoolNodes(scanner);
  SgTemplateFunctionSymbol::traverseMemoryPoolNodes(scanner);
  SgTemplateMemberFunctionSymbol::traverseMemoryPoolNodes(scanner);
}

} } }

#endif /* Rose_Source_Generation_API_txx */

