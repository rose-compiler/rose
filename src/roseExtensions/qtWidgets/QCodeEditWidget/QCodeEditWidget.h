
#ifndef Q_CODE_EDIT_WIDGET_H
#define Q_CODE_EDIT_WIDGET_H

#include "QCodeEdit/qeditor.h"
#include <QtDesigner/QDesignerExportWidget>


class QFormatScheme;
class QLanguageFactory;
class QCodeEdit;

/**
 * C++ Editor Widget, using QCodeEdit ( http://qcodeedit.edyuk.org )
 *
 * This is just derived from QEditor, and added some convenience functions
 * for detailed description look at documentation of QEditor
 */
class QDESIGNER_WIDGET_EXPORT QCodeEditWidget : public QEditor
{
    Q_OBJECT
    public:
        QCodeEditWidget(QWidget *p = 0)                   : QEditor(p)         { init(); }
        QCodeEditWidget(bool actions, QWidget *p = 0)     : QEditor(actions,p) { init(); }
        QCodeEditWidget(const QString& s, QWidget *p = 0) : QEditor(s,p)       { init(); }
        QCodeEditWidget(const QString& s, bool actions, QWidget *p = 0)
            : QEditor(s,actions,p)     { init(); }

        virtual ~QCodeEditWidget() {}


    public slots:

        /// Loads a file and forces C++ code highlighting (independent from file ending)
        /// if automatic detection of language is needed (based on filename-ending) use
        /// QEditor::load()
        void loadCppFile(const QString & filename);

        /// Sets the cursor in specified row and column
        void gotoPosition(int row, int col);

        /// Marks a line as an error (red background and error icon in linemarkpanel)
        void markAsError(int line)       { markLine("error",line); }

        /// Marks a line as a warning (yellow background and exlamation-mark icon in linemarkpanel)
        void markAsWarning(int line)     { markLine("warning",line); }

        /// Puts a breakpoint on specified line
        void markAsBreakpoint(int line)  { markLine("breakpoint",line); }


        void markLine( const QString & markStrId, int line);



        /// Displays a settings dialog, where font, tabsize etc can be changed
        static void showEditorSettingsDialog();
        /// Returns actions which can be displayed when no editor is active
        static QAction * getDisabledActions(const QString & name);

        /// Switches breakpoint editing on/off (default: off)
        void enableBreakPointEdit(bool enable=true);

        QList<int> getBreakPoints();

        QCodeEdit * getQCodeEdit() { return editorWrapper; }

    protected:

        void init();



        QCodeEdit * editorWrapper;

        QFormatScheme * m_formats;
        static QLanguageFactory * m_languages;
};

#endif
