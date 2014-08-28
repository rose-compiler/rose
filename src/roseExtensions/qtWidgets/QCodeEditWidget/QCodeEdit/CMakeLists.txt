
include_directories(. 	widgets document qnfa)

SET(QT_USE_QTXML 1)

set (EDITOR_UI_FILES 
        widgets/searchreplace.ui 
        widgets/gotoline.ui 
        widgets/editconfig.ui 
        widgets/formatconfig.ui
        )

set (EDITOR_HEADER_FILES
        qeditor.h 
        qpanellayout.h 
        qformatscheme.h 
        qlinemarksinfocenter.h 
        qreliablefilewatch.h 
        document/qdocument.h 
        qcodecompletionengine.h 
        qlanguagefactory.h 
        widgets/qpanel.h 
        widgets/qlinenumberpanel.h 
        widgets/qlinemarkpanel.h 
        widgets/qlinechangepanel.h 
        widgets/qfoldpanel.h 
        widgets/qstatuspanel.h 
        widgets/qsearchreplacepanel.h 
        widgets/qgotolinedialog.h 
        widgets/qeditconfig.h 
        widgets/qformatconfig.h 
        widgets/qsimplecolorpicker.h 
        )

set (EDITOR_CPP_FILES   
        qeditor.cpp 
        qeditorfactory.cpp 
        qcodeedit.cpp 
        qpanellayout.cpp 
        qformatscheme.cpp 
        qlinemarksinfocenter.cpp 
        qreliablefilewatch.cpp 
        document/qdocument.cpp 
        document/qdocumentcommand.cpp 
        document/qdocumentcursor.cpp 
        document/qdocumentline.cpp 
        document/qdocumentsearch.cpp 
        qcodecompletionengine.cpp 
        qlanguagedefinition.cpp 
        qlanguagefactory.cpp 
        widgets/qpanel.cpp 
        widgets/qlinenumberpanel.cpp 
        widgets/qlinemarkpanel.cpp 
        widgets/qlinechangepanel.cpp 
        widgets/qfoldpanel.cpp 
        widgets/qstatuspanel.cpp 
        widgets/qsearchreplacepanel.cpp 
        widgets/qgotolinedialog.cpp 
        widgets/qeditconfig.cpp 
        widgets/qformatconfig.cpp 
        widgets/qsimplecolorpicker.cpp 
        widgets/qcalltip.cpp   
        qnfa/qnfa.cpp
        qnfa/qnfadefinition.cpp   
        qnfa/xml2qnfa.cpp
        )
            

QT4_WRAP_CPP(EDITOR_MOC_OUTPUT ${EDITOR_HEADER_FILES})
QT4_WRAP_UI (EDITOR_UI_OUTPUT  ${EDITOR_UI_FILES} )

set (EDITOR_FILES ${EDITOR_CPP_FILES} ${EDITOR_UI_OUTPUT} ${EDITOR_MOC_OUTPUT} )

include_directories( ${CMAKE_CURRENT_BINARY_DIR} )

add_library(qcodeedit OBJECT ${EDITOR_FILES} )
set_target_properties ( qcodeedit PROPERTIES COMPILE_FLAGS "-fPIC")

