/** Base class for synthesized declarations.
 *
 *  A synthesized declaration is one created by ROSE which does not appear in the binary specimen. At one point we
 *  considered adding "synthesized" to all such intities, but later decided against it since most declarations are missing
 *  from binary specimens are are synthesized by ROSE, and would therefore lead to a lot of extra letters in many class
 *  names. */
class SgAsmSynthesizedDeclaration: public SgAsmStatement {};
