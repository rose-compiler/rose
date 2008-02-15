#include "qeditor.h"
//#include "qeditor_settings.h"
#include "qsourcecolorizer.h"
#include "cpp_colorizer.h"
#include "qeditor_indenter.h"
#include "simple_indent.h"
#include "cindent.h"
#include "parenmatcher.h"
#include "paragdata.h"

#include <private/qrichtext_p.h>
#include <qregexp.h>
#include <qmap.h>
#include <qpopupmenu.h>

using namespace std;

QEditor::QEditor( QWidget* parent, const char* name )
    : QTextEdit( parent, name )
{
    document()->setUseFormatCollection( FALSE );

    parenMatcher = new ParenMatcher();

    m_currentLine = -1;

    document()->addSelection( ParenMatcher::Match );
    document()->addSelection( ParenMatcher::Mismatch );
    document()->setSelectionColor( ParenMatcher::Match, QColor( 204, 232, 195 ) );
    document()->setSelectionColor( ParenMatcher::Mismatch, Qt::magenta );
    document()->setInvertSelectionText( ParenMatcher::Match, FALSE );
    document()->setInvertSelectionText( ParenMatcher::Mismatch, FALSE );

    document()->addSelection( 1000 );
    document()->setSelectionColor( 1000, QColor( 204, 232, 195 ) );
    
    setLanguage("c++");
    
    setTabStop(2);
    
    setWordWrap(QTextEdit::NoWrap);
    
    

    //connect( this, SIGNAL(cursorPositionChanged(QTextCursor*) ),
	//     this, SLOT(doMatch(QTextCursor*)) );
//    connect( this, SIGNAL(cursorPositionChanged(int, int) ),
//	     this, SLOT(slotCursorPositionChanged(int, int)) );
    
}

QEditor::~QEditor()
{
    delete( parenMatcher );
}


void QEditor::doMatch( QTextCursor* c )
{
    if( parenMatcher->match(c) ){
	repaintChanged();
    }
}

void QEditor::doGotoLine( int line )
{
    setCursorPosition( line, 0 );
    QTextParagraph *p = document()->paragAt( line );
    if ( !p )
	return;
    QTextCursor c( document() );
    emit ensureTextIsVisible( p );
    c.setParagraph( p );
    c.setIndex( 0 );
    document()->removeSelection( 1000 );
    document()->setSelectionStart( 1000, c );
    c.gotoLineEnd();
    document()->setSelectionEnd( 1000, c );
    viewport()->repaint( FALSE );
}

QTextCursor* QEditor::textCursor() const
{
    return QTextEdit::textCursor();
}

QTextDocument* QEditor::document() const
{
    return QTextEdit::document();
}

void QEditor::drawCursor( bool visible )
{
    QTextEdit::drawCursor( visible );
}

void QEditor::configChanged()
{
    updateStyles();
/*
    if( QEditorSettings::self()->wordWrap() ){
	setWordWrap( QEditor::WidgetWidth );
	setHScrollBarMode( QScrollView::AlwaysOff );
	setVScrollBarMode( QScrollView::AlwaysOn );
    } else {
	setWordWrap( QEditor::NoWrap );
	setHScrollBarMode( QScrollView::AlwaysOn );
	setVScrollBarMode( QScrollView::AlwaysOn );
    }
*/
    refresh();
}


void QEditor::updateStyles()
{
    //kdDebug(9032) << "QEditor::updateStyles()" << endl;
    int tabwidth = tabStop();
    QSourceColorizer* colorizer = dynamic_cast<QSourceColorizer*>( document()->preProcessor() );
    if( colorizer ){
	setFont( colorizer->format( 0 )->font() );
	setTabStopWidth( colorizer->format(0)->width('x') * tabwidth );
	document()->setTabStops( colorizer->format(0)->width('x') * tabwidth );
    }
    QTextEdit::updateStyles();
}


void QEditor::setDocument( QTextDocument* doc )
{
    QTextEdit::setDocument( doc );
}

void QEditor::repaintChanged()
{
    QTextEdit::repaintChanged();
}

QString QEditor::textLine( uint line ) const
{
    return text( line );
}

void QEditor::setLanguage( const QString& l )
{
//    kdDebug(9032) << "QEditor::setLanguage(" << l << ")" << endl;
    m_language = l;
    if( m_language == "c++" ){
        setElectricKeys( "{}" );
	document()->setPreProcessor( new CppColorizer(this) );
	document()->setIndent( new CIndent(this) );
    } else {
        setElectricKeys( QString::null );
	document()->setPreProcessor( 0 );
	document()->setIndent( new SimpleIndent(this) );
    }

    configChanged();
    sync();
}

QString QEditor::language() const
{
    return m_language;
}

void QEditor::setText( const QString& text )
{
    setTextFormat( QTextEdit::PlainText );
    QString s = text;
    // tabify( s );
    QTextEdit::setText( s );
    setTextFormat( QTextEdit::AutoText ); 
}

void QEditor::slotCursorPositionChanged( int line, int column )
{
    Q_UNUSED( line );
    Q_UNUSED( column );
}

int QEditor::level( int line) const
{
    ParagData* data = (ParagData*) document()->paragAt( line )->extraData();
    if( data ){
	return data->level();
    }
    return 0;
}


void QEditor::setLevel( int line, int lev )
{
    ParagData* data = (ParagData*) document()->paragAt( line )->extraData();
    if( data ){
        return data->setLevel( lev );
    }
}

QSourceColorizer* QEditor::colorizer() const
{
    return dynamic_cast<QSourceColorizer*>( document()->preProcessor() );
}

void QEditor::refresh()
{
    document()->invalidate();
    QTextParagraph* p = document()->firstParagraph();
    while( p ){
        p->format();
	p = p->next();
    }
    removeSelection( ParenMatcher::Match );
    removeSelection( ParenMatcher::Mismatch );
    ensureCursorVisible();
    repaintContents( false );
}


QEditorIndenter* QEditor::indenter() const
{
  return dynamic_cast<QEditorIndenter*>( document()->indent() );
}

void QEditor::indent()
{
    QTextEdit::indent();
    if( !hasSelectedText() && text( textCursor()->paragraph()->paragId() ).stripWhiteSpace().isEmpty() )
	moveCursor( MoveLineEnd, false );
}

void QEditor::contentsMouseDoubleClickEvent( QMouseEvent * e )
{ 

    if ( e->button() != Qt::LeftButton ) {
        e->ignore();
        return;
    }

    int para = 0;
    int index = charAt( e->pos(), &para );
    
    emit doubleClicked(para, index);
        
}
 
bool QEditor::isDelimiter(const QChar& c)
{
    if (c == '_') return false;
    return !(c.isLetterOrNumber());
}

void QEditor::doSelectCode(int line0, int col0, int line1, int col1) {
    document()->removeSelection(1000);
    setCursorPosition(line0, col0);
    QTextCursor c1 = *textCursor();
    
    setCursorPosition(line1, col1);
    QTextCursor c2 = *textCursor();
    
    document()->setSelectionStart( 1000, c1 );
    document()->setSelectionEnd( 1000, c2 );

    repaintChanged();
}

void QEditor::contentsMousePressEvent ( QMouseEvent * e ) {
    if (e->button() != Qt::RightButton) {
	e->ignore();
	return;
    }
    int line = 0;
    int col = charAt(e->pos(), &line);
    emit rightClicked(e->globalPos(), line, col);
}

int QEditor::tabStop() const {
    return m_tabStop;
}
void QEditor::setTabStop( int tab) {
    m_tabStop = tab;
}

QPopupMenu *QEditor::createPopupMenu(const QPoint &pos) {
    return NULL;
}
#include "qeditor.moc"
