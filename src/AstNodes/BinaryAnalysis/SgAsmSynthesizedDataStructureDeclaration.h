#include <sageContainer.h>

/** Declaration-like nodes that encapsulate multiple instructions.
 *
 *  Binary ASTs have two sides: the container side that corresponds to the ELF/PE/etc. file formats, and the interpretation
 *  side that corresponds to instructions and data from multiple sources (specimen + dynamic libraries) organized into
 *  multiple SgAsmInterpretation where each interpretation makes a coherent binary entity such as the DOS part of a PE
 *  executable.  The declaration-like nodes that follow appear on the interpretation side of the AST.  We may add other
 *  declaration nodes to the container side of the AST at a later time.
 *
 *  These interpretation-side declaration-like nodes are used by the projects/BinaryDataStructureRecognition even if they
 *  aren't used internally by ROSE. */
class SgAsmSynthesizedDataStructureDeclaration: public SgAsmSynthesizedDeclaration {
protected:
      SgAsmSynthesizedDeclarationPtrList p_declarationList;

public:
    /** Appends another declaration. */
    void appendDeclaration(SgAsmSynthesizedDeclaration *declaration) {
        p_declarationList.push_back(declaration);
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Deprecated 2023-11
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
    void append_declaration(SgAsmSynthesizedDeclaration*) ROSE_DEPRECATED("use appendDeclaration");
};
