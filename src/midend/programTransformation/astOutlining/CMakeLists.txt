

install(FILES
    Outliner.hh
    NameGenerator.hh
    ASTtools.hh
    Copy.hh
    Jumps.hh
    PrePostTraversal.hh
    PreprocessingInfo.hh
    StmtRewrite.hh
    This.hh
    VarSym.hh
    IfDirectiveContextFinder.hh
    IfDirectiveExtractor.hh
    If.hh
    Preprocess.hh
    outlining_lib.h
    DESTINATION
    ${INCLUDE_INSTALL_DIR}
)

add_library(outlining STATIC outlining_lib.c)

install(TARGETS outlining
    ARCHIVE DESTINATION ${LIB_INSTALL_DIR}
)