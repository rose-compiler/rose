
include_directories(${KDE4_INCLUDES} ${KDE4_INCLUDE_DIR} ${QT_INCLUDES} )

install(FILES Assert.h BitVector.h BitVectorSupport.h CommandLine.h Graph.h GraphBoost.h 
              IndexedList.h Interval.h Map.h Markup.h MarkupRoff.h Message.h ProgressBar.h Sawyer.h
        DESTINATION ${INCLUDE_INSTALL_DIR}/sawyer)
