#ifndef CINDENT_H
#define CINDENT_H

#include "qeditor_indenter.h"
#include "dlldefs.h"

class EDITOR_EXPORT CIndent : public QEditorIndenter
{
public:
    CIndent( QEditor* );
    virtual ~CIndent();

    virtual int indentForLine( int line );
//    virtual QWidget* createConfigPage( QEditorPart*, KDialogBase* =0, const char* =0 );
    virtual void updateValues( const QMap<QString, QVariant>& );
};

#endif
