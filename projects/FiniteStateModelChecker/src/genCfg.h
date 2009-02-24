#ifndef GENCFG_H
#define GENCFG_H


#include <gcj/cni.h>
#include <mops/CfgFunction.h>
#include <mops/Ast.h>
#include <mops/Cfg.h>

mops::CfgFunction *MopsBuildFunction(SgFunctionDeclaration *decl);
mops::Ast *MopsBuildVariable(SgVariableDeclaration *decl);
mops::Cfg *MopsBuildProject(SgProject *project);

#endif
