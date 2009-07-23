
#ifndef ROSECODEEDIT_H
#define ROSECODEEDIT_H

#include "QCodeEdit/qeditor.h"
#include <QtDesigner/QDesignerExportWidget>

class SgNode;

class QFormatScheme;
class QLanguageFactory;
class QCodeEdit;

/**
 * C++ Editor Widget, using QCodeEdit ( http://qcodeedit.edyuk.org )
 *
 * This is just derived from QEditor, and added some convenience functions
 * for detailed description look at documentation of QEditor
 */
class QDESIGNER_WIDGET_EXPORT RoseCodeEdit : public QEditor
{
    Q_OBJECT
    public:
        RoseCodeEdit(QWidget *p = 0)                   : QEditor(p)         { init(); }
        RoseCodeEdit(bool actions, QWidget *p = 0)     : QEditor(actions,p) { init(); }
        RoseCodeEdit(const QString& s, QWidget *p = 0) : QEditor(s,p)       { init(); }
        RoseCodeEdit(const QString& s, bool actions, QWidget *p = 0)
            : QEditor(s,actions,p)
        { init(); }

        virtual ~RoseCodeEdit() {}


    public slots:
        /// If the specified sgNode is a SgLocatedNode, the right file is opened
        /// and the cursor is set to start position of this SgNode
        /// If node is SgFile the file is opened and cursor is set to beginning of file
        void setNode(SgNode * sgNode);
        // same as setNode, for convenience with the link mechanism
        void gotoNode( SgNode *sgNode )
        { setNode( sgNode ); }

        /// Loads a file and forces C++ code highlighting (independent from file ending)
        /// if automatic detection of language is needed (based on filename-ending) use
        /// QEditor::load()
        void loadCppFile(const QString & filename);

        /// Sets the cursor in specified row and column
        void gotoPosition(int row, int col);

        /// Marks a line as an error (red background and error icon in linemarkpanel)
        void markAsError(int line);

        /// Marks a line as a warning (yellow background and exlamation-mark icon in linemarkpanel)
        void markAsWarning(int line);

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

        virtual void dragEnterEvent(QDragEnterEvent * ev);
        virtual void dropEvent(QDropEvent * ev);

        QCodeEdit * editorWrapper;

        QFormatScheme * m_formats;
        static QLanguageFactory * m_languages;
};

#endif
