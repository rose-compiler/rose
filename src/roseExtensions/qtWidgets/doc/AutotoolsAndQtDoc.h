/**
\page autotools  Setup Qt with autotools buildsystem

    \section autotools_setup Setup of BuildSystem

    Since ROSE uses the autotools buildsystem, and it's not trivial to build Qt applications with autotools
    a include file for your Makefile is available. It contains all rules for invocation of Qt's metacompilers
    and sets the \c INCLUDES variable correctly.
    You can find this file at \c src/roseExtensions/qtWidgets/Make.inc

    Template for your Makefile.am
    \verbatim
        include $(top_srcdir)/src/roseExtensions/qtWidgets/Make.inc

        # Compile only when configured with --with-roseQt
        if ROSE_WITH_ROSEQT

        # Includes have to use += , because INCLUDES is already set in Make.inc
        INCLUDES += $(ROSE_INCLUDES)

        endif
    \endverbatim

    \subsection makefile_mocfiles Usage of  moc
        You can find the general documentation of \c moc here: http://doc.qtsoftware.com/4.5/moc.html \n

        Example Makefile.am - entry, assuming you have a \c Widget.h and \c Widget.cpp

        \verbatim
            yourBin_SOURCES = Widget.cpp
            nodist_yourBin_SOURCES = moc_Widget.cpp
        \endverbatim


    \subsection makefile_uifiles  Handling of ui-files

        To get an idea of what the \c uic does, and how to use it look at
        http://doc.qtsoftware.com/4.5/uic.html and http://doc.qtsoftware.com/4.5/designer-using-a-ui-file.html

        Example Makefile.am, assuming you have a Widget.ui
        \verbatim
            nodist_yourBin_SOURCES = ui_Widget.h

            BUILT_SOURCES = ui_Widget.h
        \endverbatim

        \b Warning: This setup described above is not optimal. The dependency of \c Widget.h on \c Widget.ui is not handled right.\n
                    The \c ui_Widget.h is generated once in the \c all target.
                    When you've changed the \c .ui file you have to do a \c make \c clean first.

    \subsection makefile_resfiles  Handling of resource-files
        If you want to use custom icons, or other resources you can use Qt's resourcecompiler \c rcc. \n
        A detailed description how to use resource files you can find here: http://doc.trolltech.com/4.5/resources.html

        Example Makefile.am, assuming your file is named res.qrc
        \verbatim
            nodist_yourBin_SOURCES = qrc_res.cpp
        \endverbatim

*/
