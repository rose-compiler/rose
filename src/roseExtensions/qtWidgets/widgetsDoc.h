/**
@mainpage RoseQtWidgets

\section Overview

    \c libRoseQtWidgets.so provides Qt-Widgets which can display internal datastructures of ROSE. \n

    <img src="../overview.jpg"  alt="Screenshot">




\section qtdesigner_integration     Qt-Designer Integration

    In order to use the RoseWidgets with QtDesigner, the widgets-library has to
    be copied or symlinked  into the Qt-Plugins directory:

    \verbatim
     ln -s  roseInstallDir/lib/libRoseQtWidgets.so \
            qtinstallpath/qtplugins/designer/
    \endverbatim

    The default Qt-Plugin path is  \em QtInstallationFolder/qtplugins . If you do not have
    write access to that, it's possible to create a new one and set the environment
    variable QT_PLUGIN_PATH accordingly. \n
    \b Important: the env-variable has to point to the \em qtplugins folder not to the \em designer folder.


\section autotools     Qt's Metacompiler and Autotools

    When you write an application using qt, you need to setup Qt's metacompiler correctly.
    Therefore you can include the file \c projects/roseQt/UseQtWidgets.inc in your Makefile.am . \n
    It contains the rules for invocation of the metacompilers.

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


\section Usage

    The next 3 section describe three different approaches for using roseQtWidgets. \n
    They are the same as for any other Qt-Widget, so you can find a more detailed description at http://doc.qtsoftware.com/4.5/designer-using-a-ui-file.html \n
    The following sections are a short summary of this document, and focus especially on the usage  of \em roseQtWidgets with \em qrose.

    \subsection manually         Create Manually

        The easiest (but not very comfortable) way to use RoseQtWidgets is by just
        linking against the \c libRoseQtWidgets.so and create the widgets yourself.

        An example is provided in the \c demo/main.cpp , function setupGuiManually() .


    \subsection uic             uic  - Qt's Ui-Compiler

        The next approach uses Qt-Designer, therefore the plugin has to be properly installed. \n
        After creating a custom widget with Qt-Designer, the ui files can be
        compiled with \em uic, which generates a \em .h file.

        Usage example:

        \code
        #include "ui_UiFile.h"
        ...
        {
            Ui::NameFromQtDesigner uiObject;
            QWidget * myWidget = new QWidget();
            uiObject.setupUi(myWidget);
            // Access to a subwidget:
            uiObject.myButton->setText("NewCaption");
        }
        \endcode

        The created widget can easily be added to a QRose Window.
        \code
        QRWindow * window = new QRWindow ();
        (*window)["WindowTitle"] << myWidget;
        \endcode

        For further examples see setupGuiWithCompiledUi() and MainWindow.cpp of the demo-application.

    \subsection runtime         Load .ui at runtime

        There is a possibility to load .ui files at run-time, so there is no need for
        using the uic-compiler.

        Usage example: ( see function \em setupGuiUiFile() in demo/main.cpp )
        \code
            QUiLoader loader;
            QFile fileStream("myUiFile.ui");
            fileStream.open(QFile::ReadOnly);

            QWidget * loadedWin = loader.load(&fileStream);

            // Access to a subwidget:
            QPushButton * myButton = loadedWin->findChild<QPushButton*>("NameOfButton");
            myButton->setText("NewCaption");
        \endcode


        This ui-loading code is now also integrated into qRose:
        \code
            (*qrWindow)["customWdg"] << std::string("myUiFile.ui");
        \endcode

        \b Warning: On some architectures there may be problems with the initialization of static variables in the dynamic loaded lib.
                    So there  a crash when using NodeInfoWidget to display file information which is stored in static vars in librose.

    \section demo Demo Application
        A demo application which uses all roseQt-Widgets is available at \c projects/roseQt/demo

    \section arrow_preview Drawing Arrows in QTextEdit - Preview
        A preview of how to draw arrows into QTextEdit's (planned to integrate in Source-Views) is at \c projects/roseQt/TextEditPainter


    \section all_widgets All Widgets:

        Here is a description of all included widgets:
            - AstBrowserWidget\n   \copydetails AstBrowserWidget
            - AstGraphWidget\n     \copydetails AstGraphWidget
            - BAstView\n           \copydetails BAstView
            - KiviatView\n         \copydetails KiviatView
            - MetricsKiviat\n      \copydetails MetricsKiviat
            - MetricsConfig\n      \copydetails MetricsConfig
            - MetricFilter\n       \copydetails MetricFilter
            - PropertyTreeWidget\n \copydetails PropertyTreeWidget
            - NodeInfoWidget\n     \copydetails NodeInfoWidget
            - RoseFileComboBox\n   \copydetails RoseFileComboBox
            - RoseCodeEdit\n       \copydetails RoseCodeEdit


\author Martin Bauer, Thomas Heller

*/


