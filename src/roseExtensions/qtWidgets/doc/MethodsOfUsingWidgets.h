/**

 \page methods_of_usage  Different approaches howto use Qt-Widgets


    The next 3 section describe three different approaches for using roseQtWidgets. \n
    They are the same as for any other Qt-Widget, so you can find a more detailed description at http://doc.qtsoftware.com/4.5/designer-using-a-ui-file.html \n
    The following sections are a short summary of this document, and focus especially on the usage  of \em roseQtWidgets with \em qrose.

    \section manually         Create Manually

        The easiest (but not very comfortable) way to use RoseQtWidgets is by just
        linking against the \c libRoseQtWidgets.so and create the widgets yourself.



    \section uic             uic  - Qt's Ui-Compiler (suggested)

        The next approach uses Qt-Designer, therefore the plugin has to be properly installed.
        How to setup the integration is described  \ref qtdesigner_integration "here" \n
        After creating a custom widget with Qt-Designer, the ui files can be
        compiled with \em uic, which generates a \em .h file.
        The setup of autotools Makefile.am to use the uic is described \ref makefile_uifiles "here"

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


    \section runtime         Load .ui at runtime

        There is a possibility to load .ui files at run-time, so there is no need for
        using the uic-compiler.

        Usage example:
        \code
            QUiLoader loader;
            QFile fileStream("myUiFile.ui");
            fileStream.open(QFile::ReadOnly);

            QWidget * loadedWin = loader.load(&fileStream);

            // Access to a subwidget:
            QPushButton * myButton = loadedWin->findChild<QPushButton*>("NameOfButton");
            myButton->setText("NewCaption");
        \endcode

        \b Warning: On some architectures there may be problems with the initialization of static variables in the dynamic loaded lib.
                    So there  a crash when using NodeInfoWidget to display file information which is stored in static vars in librose.
*/
