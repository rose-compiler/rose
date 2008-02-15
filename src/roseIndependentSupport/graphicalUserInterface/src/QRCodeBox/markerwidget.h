#ifndef __markerwidget_h
#define __markerwidget_h

#include <qwidget.h>
#include <qpixmap.h>
#include <qmap.h>

//# include <ktexteditor/markinterfaceextension.h>

class QEditor;

class Mark {
public:
    int line;
    int type;      
};

class MarkerWidget: public QWidget
{
    Q_OBJECT
    
public:
    MarkerWidget( QEditor*, QWidget* =0, const char* =0 );
    virtual ~MarkerWidget();

    virtual void setPixmap(int, const QPixmap &);
    virtual void setDescription(int, const QString &);
    virtual void setMarksUserChangable(uint markMask);

public slots:
    void doRepaint() { repaint( FALSE ); }

protected:
    virtual void resizeEvent( QResizeEvent* );
    virtual void paintEvent( QPaintEvent* );
    virtual void contextMenuEvent( QContextMenuEvent* );
    virtual void mousePressEvent ( QMouseEvent * e );

signals:
    void marksChanged();

private:
    QEditor*                m_editor;
    QPixmap                 m_buffer;
    QMap<int,QPixmap>       m_pixmapMap;
    bool                    m_clickChangesBPs;
    bool                    m_changeBookmarksAllowed;
    bool                    m_changeBreakpointsAllowed;
    QString                 m_bookmarkDescr;
    QString                 m_breakpointDescr;
};

#endif // __markerwidget_h
