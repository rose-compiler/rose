
#ifndef ROSECODEEDIT_H
#define ROSECODEEDIT_H

#include "QCodeEdit/qeditor.h"
#include <QtDesigner/QDesignerExportWidget>

class SgNode;

class QFormatScheme;
class QLanguageFactory;

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
        void setNode(SgNode * sgNode);

        void loadCppFile(const QString & filename);
        void gotoPosition(int row, int col);

        void markAsError(int line);
        void markAsWarning(int line);


    protected:
        void init();

       QFormatScheme    * m_formats;
       static QLanguageFactory * m_languages;
};

#endif
