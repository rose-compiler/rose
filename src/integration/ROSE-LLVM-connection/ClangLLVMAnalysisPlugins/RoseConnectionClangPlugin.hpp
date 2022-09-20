#ifndef ROSE_CONNECTION_CLANG_PLUGIN_H                                        

#define ROSE_CONNECTION_CLANG_PLUGIN_H

#include "clang/Frontend/FrontendPluginRegistry.h"
#include "clang/AST/AST.h"
#include "clang/AST/ASTConsumer.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Sema/Sema.h"
#include "llvm/Support/raw_ostream.h"

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/optional.hpp>

using namespace clang;

boost::property_tree::ptree pt;

#endif // ROSE_CONNECTION_CLANG_PLUGIN_H
