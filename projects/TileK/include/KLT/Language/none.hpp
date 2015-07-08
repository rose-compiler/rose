
#ifndef __KLT_LANGUAGE_NONE_HPP__
#define __KLT_LANGUAGE_NONE_HPP__

class SgFunctionDeclaration;

namespace KLT {

namespace Language {

struct None {
  static void applyKernelModifiers(SgFunctionDeclaration * kernel_decl);
};

} // namespace KLT::Language

} // namespace KLT

#endif /* __KLT_LANGUAGE_NONE_HPP__ */

