#ifndef qeditor_h
#define qeditor_h

#include <qtextedit.h>
#include <qstringlist.h>
#include <qptrlist.h>

class ParenMatcher;
class QPopupMenu;
class QSourceColorizer;
class QEditorIndenter;
class QEditorKey;

class QEditor: public QTextEdit
{
    Q_OBJECT
    
public:
    QEditor( QWidget* parent=0, const char* name=0 );
    virtual ~QEditor();

    QEditorIndenter* indenter() const;
    QTextCursor* textCursor() const;
    QTextDocument* document() const;
    void setDocument( QTextDocument*);

    void drawCursor( bool visible );
    void updateStyles();
    void repaintChanged();

    virtual QString electricKeys() const { return m_electricKeys; }
    virtual void setElectricKeys( const QString& keys ) { m_electricKeys = keys; }

    virtual int tabStop() const;
    virtual void setTabStop( int );

    virtual QString textLine( uint ) const;

    virtual void setLanguage( const QString& );
    virtual QString language() const;

    virtual void setText( const QString& );

    virtual int level( int ) const;
    virtual void setLevel( int, int );
   
    virtual void indent();

    QSourceColorizer* colorizer() const;
    
    virtual void doSelectCode(int line0, int col0, int line1, int col1);
        
signals:
    void parsed();
    void ensureTextIsVisible( QTextParagraph* );
    void rightClicked(const QPoint &, int, int);

public slots:
    void doGotoLine( int line );
    virtual void configChanged();
    virtual void refresh();

private slots:
    void doMatch( QTextCursor* );
    void slotCursorPositionChanged( int, int );

protected:
    void contentsMouseDoubleClickEvent( QMouseEvent * e );    
    void contentsMousePressEvent ( QMouseEvent * e );     
    bool isDelimiter(const QChar& c);
    QPopupMenu *createPopupMenu(const QPoint &pos);
    
private:
    ParenMatcher* parenMatcher;
    QString m_language;
    int m_currentLine;
    int m_tabStop;
    QString m_electricKeys;
};

#endif
