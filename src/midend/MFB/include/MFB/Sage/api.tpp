/** 
 * \file MFB/include/MFB/api.tpp
 *
 * \author Tristan Vanderbruggen
 *
 */

#ifndef _MFB_API_TPP_
#define _MFB_API_TPP_

#include "sage3basic.h"

namespace MFB {

template <typename CRT>
void API<CRT>::load(Driver<Sage> & driver) {
  load_headers(driver);
  load_api(driver);
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

  out << "  Types:" << std::endl;
  for (auto p: types) {
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
void API<CRT>::build_command_line(std::vector<std::string> & cmdline) const {
  cmdline.push_back("rose-cxx");
  cmdline.push_back("-c");

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
}

template <typename CRT>
void API<CRT>::load_headers(Driver<Sage> & driver) {
//  std::cerr << "ENTER: API<CRT>::load_headers" << std::endl;

  if (!CommandlineProcessing::isCppFileNameSuffix("hxx")) {
    CommandlineProcessing::extraCppSourceFileSuffixes.push_back("hxx");
  }

  std::vector<std::string> old_cmdline = driver.project->get_originalCommandLineArgumentList();
  std::vector<std::string> cmdline;
  build_command_line(cmdline);
  driver.project->set_originalCommandLineArgumentList(cmdline);

  for (auto file: files) {
//    std::cerr << "  * loading:  " << file << std::endl;
    bool found = false;
    if (!cache.empty()) {
      // TODO Try to get AST file from cache directory else build AST file
      // TODO Need "driver.readAST(fp);" which could return a set of file_id...
    }

    if (!found) {
      for (auto path: paths) {
        auto fp = path + "/" + file;
//        std::cerr << "  * checking: " << fp << std::endl;
        if (boost::filesystem::exists(fp)) {
          driver.add(fp);
//          std::cerr << "  * Found!" << std::endl;
          found = true;
          break;
        }
      }
    }

    if (!found) {
      std::cerr << "[WARN] API header file not found: " << file << std::endl;
    }
  }

  driver.project->set_originalCommandLineArgumentList(old_cmdline);

  ROSE_ASSERT(SageBuilder::topScopeStack() == NULL); // Sanity check

//  std::cerr << "LEAVE: API<CRT>::load_headers" << std::endl;
}

template <typename CRT>
void API<CRT>::load_api(Driver<Sage> & driver) {
//std::cerr << "ENTER: API<CRT>::load_api" << std::endl;

  api_t * api = driver.getAPI();

  // Namespaces
  for (auto sym: api->namespace_symbols) {
    auto str = sym->get_declaration()->get_qualified_name().getString();
//  std::cerr << "  NSPC: " << str << std::endl;
    for (auto p: namespaces) {
//    std::cerr << "    -> " << p.first << std::endl;
//    std::cerr << "    -> " << (str == p.first ? "equ" : "dif") << std::endl;
    }
    auto it = namespaces.find(str);
    if (it != namespaces.end()) {
      SgNamespaceDeclarationStatement * nspc_decl = sym->get_declaration();
      ROSE_ASSERT(nspc_decl != NULL);

//    std::cerr << "  NSPC: " << str << " -> " << std::hex << nspc_decl << std::endl;

      ((CRT*)this)->*(it->second) = nspc_decl;
    }
  }

  // Types from Typedefs
  for (auto sym: api->typedef_symbols) {
    auto str = sym->get_declaration()->get_qualified_name().getString();
//  std::cerr << "  Typedef: " << str << std::endl;
    auto it = types.find(str);
    if (it != types.end()) {
      SgTypedefDeclaration * tdef_decl = sym->get_declaration();
      ROSE_ASSERT(tdef_decl != NULL);
      SgTypedefType * tdef_type = isSgTypedefType(tdef_decl->get_type());
      ROSE_ASSERT(tdef_type != NULL);

      ((CRT*)this)->*(it->second) = tdef_type;
    }
  }
  // Types from Classes
  for (auto sym: api->class_symbols) {
    auto str = sym->get_declaration()->get_qualified_name().getString();
//  std::cerr << "  Class: " << str << std::endl;
    auto it = types.find(str);
    if (it != types.end()) {
      SgClassDeclaration * xdecl = sym->get_declaration();
      ROSE_ASSERT(xdecl != NULL);
      SgClassType * xtype = isSgClassType(xdecl->get_type());
      ROSE_ASSERT(xtype != NULL);

      ((CRT*)this)->*(it->second) = xtype;
    }
  }

  // Variables
  for (auto sym: api->variable_symbols) {
    auto str = sym->get_declaration()->get_qualified_name().getString();
//  std::cerr << "  Variable: " << str << std::endl;
    auto it = variables.find(str);
    if (it != variables.end()) {
      ((CRT*)this)->*(it->second) = sym;
    }
  }

  // Functions
  for (auto sym: api->function_symbols) {
    auto str = sym->get_declaration()->get_qualified_name().getString();
//  std::cerr << "  Function: " << str << std::endl;
    auto it = functions.find(str);
    if (it != functions.end()) {
      ((CRT*)this)->*(it->second) = sym;
    }
  }

//std::cerr << "LEAVE: API<CRT>::load_api" << std::endl;
}

}

#endif /* _MFB_API_TPP_ */

