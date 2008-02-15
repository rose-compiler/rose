#include "qeditor_indenter.h"
#include "qeditor.h"
//#include <kdebug.h>

QEditorIndenter::QEditorIndenter( QEditor* ed )
    : m_editor( ed )
{
}

QEditorIndenter::~QEditorIndenter()
{
}

int QEditorIndenter::indentation( const QString &s )
{
    if ( s.simplifyWhiteSpace().length() == 0 )
        return 0;
    
    int tabwidth = m_editor->tabStop();
    int i = 0;
    int ind = 0;
    while ( i < (int)s.length() ) {
        QChar c = s.at( i );
        if ( c == ' ' ){
            ind++;
        } else if ( c == '\t' ){
            ind += tabwidth;
        } else {
            break;
        }
        ++i;
    }
    return ind;
}

void QEditorIndenter::indentLine( QTextParagraph *p, int &oldIndent, int &newIndent )
{
    QString indentString;
    indentString.fill( ' ', newIndent );
    indentString.append( "a" );
    tabify( indentString );
    indentString.remove( indentString.length() - 1, 1 );
    newIndent = indentString.length();
    oldIndent = 0;
    while ( p->length() > 0 && ( p->at( 0 )->c == ' ' || p->at( 0 )->c == '\t' ) ) {
        ++oldIndent;
        p->remove( 0, 1 );
    }
    if ( p->string()->length() == 0 )
        p->append( " " );
    if ( !indentString.isEmpty() )
        p->insert( 0, indentString );
}

void QEditorIndenter::tabify( QString& s )
{
    int i = 0;
    int tabSize = m_editor->tabStop();
    for ( ;; ) {
        for ( int j = i; j < (int)s.length(); ++j ) {
            if ( s[ j ] != ' ' && s[ j ] != '\t' ) {
                if ( j > i ) {
                    QString t  = s.mid( i, j - i );
                    int spaces = 0;
                    for ( int k = 0; k < (int)t.length(); ++k )
                        spaces += ( t[ k ] == ' ' ? 1 : tabSize );
                    s.remove( i, t.length() );
                    int tabs = spaces / tabSize;
                    spaces = spaces - ( tabSize * tabs );
                    QString tmp;
                    tmp.fill( ' ', spaces );
                    if ( spaces > 0 )
                        s.insert( i, tmp );
                    tmp.fill( '\t', tabs );
                    if ( tabs > 0 )
                        s.insert( i, tmp );
                }
                break;
            }
        }
        i = s.find( '\n', i );
        if ( i == -1 )
            break;
        ++i;
    }
}

void QEditorIndenter::indent( QTextDocument*, QTextParagraph* parag, int* oldIndent, int* newIndent )
{
    int line = parag->paragId();
    QString text = m_editor->text( line );

    int oi = indentation( text );
    
    int ind = indentForLine( line );

    indentLine( parag, oi, ind );

    if( oldIndent ) *oldIndent = oi;
    if( newIndent ) *newIndent = ind;
}

int QEditorIndenter::previousNonBlankLine( int line )
{
    while( --line >=0 ){
	if( !editor()->text( line ).stripWhiteSpace().isEmpty() )
	    break;
    }
    return line;
}

