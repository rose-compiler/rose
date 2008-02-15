#ifndef QRCODEBOX_H
#define QRCODEBOX_H

#include <qwidget.h>
#include <QRoseGroup.h>
#include <QRMacros.h>

class QTextParagraph;
class QEditor;
class LineNumberWidget;
class LevelWidget;
//class MarkerWidget;
class QRAstCoords;
class QPopupMenu;
class QStatusBar;
class QVBoxLayout;
class QRToolBar;
class QTabBar;

class QRCodeBox: public QWidget, public QRoseGroupWidget  {

    Q_OBJECT
    
public:
    
    typedef enum toolbar_id { tb_cache_view, tb_clear };
    
    QRCodeBox(const char *name = 0);
    QRCodeBox(QWidget *parent, const char *name = 0);
    ~QRCodeBox();
    void receiveNode(QRGroupWidget *emitter, SgNode *node);    
    void contentsMouseDoubleClickEvent( QMouseEvent * e );
    

public slots:
    void expandBlock( QTextParagraph* );
    void collapseBlock( QTextParagraph* );
    void broadcastTargetNode(int);
    void listSgNodes(const QPoint &point, int line, int col);    
    void toolbarClicked(int, bool);

protected:
    void init();
    void receiveMsg(QRGroupWidget *emitter, QRMessage *msg);
    void loadCache();
        
    void doRepaint();
    void internalExpandBlock( QTextParagraph* );
    void internalCollapseBlock( QTextParagraph* );
    virtual void eventCacheModified();    


protected:
    QEditor *m_editor;
    LineNumberWidget *m_lineNumberWidget;
    LevelWidget *m_levelWidget;
    QRAstCoords *m_ast_coords;
    QPopupMenu *m_popup;
    QStatusBar *m_statusbar;  
    QVBoxLayout *m_layout;   
    QR_EXPORT(QRToolBar,Toolbar);
    QTabBar *m_tabbar;

    //MarkerWidget *m_markerWidget;
};

#endif
