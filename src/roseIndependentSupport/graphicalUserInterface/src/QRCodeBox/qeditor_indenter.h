
#ifndef QEDITOR_INDENTER_H
#define QEDITOR_INDENTER_H

#include <qstring.h>
#include <qvariant.h>
#include <qmap.h>
#include <private/qrichtext_p.h>

class KDialogBase;
class QEditor;

class QEditorIndenter: public QTextIndent{
public:
    QEditorIndenter( QEditor* );
    virtual ~QEditorIndenter();

//    virtual QWidget* createConfigPage( QEditorPart*, KDialogBase* =0, const char* =0 ) { return 0; }

    virtual const QMap<QString, QVariant>& values() const { return m_values; }
    virtual void updateValues( const QMap<QString, QVariant>& values ) { m_values = values; }

    QEditor* editor() const { return m_editor; }
    
    virtual void tabify( QString& text );
    virtual void indentLine( QTextParagraph *p, int &oldIndent, int &newIndent );
    virtual int indentation( const QString& s );

    virtual int previousNonBlankLine( int line );
    virtual int indentForLine( int line ) = 0;
    virtual void indent( QTextDocument*, QTextParagraph*, int* =0, int* =0 );

protected:
    QEditor* m_editor;
    QMap<QString, QVariant> m_values;
};


#endif
