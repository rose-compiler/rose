/*
 * HaOptUtil.hpp
 *
 *  Created on: Oct 30, 2012
 *      Author: Kamal Sharma
 */

#ifndef HAOPTUTIL_HPP_
#define HAOPTUTIL_HPP_

namespace HAOPTUtil {

bool DoPointer(SgInitializedName *name, SgStatement *scope, CompilerTypes::TYPE compiler, bool applyRestrict);
bool DoArray(SgInitializedName *name, SgStatement *scope, CompilerTypes::TYPE compiler);
void DoPragma(SgProject *project, CompilerTypes::TYPE compilerType);
void DoStructDecl(SgInitializedName *name, bool applyRestrict);

}


#endif /* HAOPTUTIL_HPP_ */
