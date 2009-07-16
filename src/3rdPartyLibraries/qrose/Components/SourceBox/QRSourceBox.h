/***************************************************************************
 *   Copyright (C) 2005 by Jose G de Figueiredo Coutinho                   *
 *   jgfc@doc.ic.ac.uk                                                     *
 ***************************************************************************/

#ifndef QRCODEBOX_H
#define QRCODEBOX_H

#include <Qt3Support>
#include <QtGui>
#include <QtCore>

#include <QRoseComponent.h>
#include <QRMacros.h>

class SgFile;
class SgNode;

/**
* \ingroup qrose_comps
*/

namespace qrs {
class QRAstCoords;
class QRToolBar;
class QTabBar;
class QRCoordBox;
class Q3TextParagraph;
class QEditor;
class LineNumberWidget;
class LevelWidget;

/// Component that implements ROSE IR Unparser
class QRSourceBox: public QWidget, public QRoseComponent  {

    Q_OBJECT

public:
	enum ToolbarIdx { tbTargetMode = 0, tbCacheView, tbCollectMode, tbClearCache, tbBroadcastCache, tbGoto, tbLookup, tbZoomIn, tbZoomOut };


    //@{

            /// @name Initialization

            /// Creates a code-box associated to group 0 in cache-view only
    QRSourceBox();
            /// Creates a code-box associated to group 0 and unparses file @a root
    QRSourceBox(SgNode *root);

    ~QRSourceBox();

    //@}

    //@{
            /// @name Component properties

            /// Returns true if its in cache-view mode
    bool isCacheView();
            /// Returns true if component finds source location when a node is received
    bool isLookup();
            /// Returns root node associated with this component
    SgNode *getRoot();

    //@}

    //@{
             /// @name Component settings

    void showToolbar(bool setOn);
            /// Sets to highlight source-location for every node received
    void setLookupWithHighlight(bool isOn);
    //@}


public slots:
    void setTargetMode(bool isOn);
    void setCollectMode(bool isOn);
    void clearCache();
    void broadcastCache();
            /// Set cache view mode [shows nodes in cache]
    void setCacheView(bool isOn);
            /// Sets component to lookup source position of node received
    void setLookup(bool isOn);
            /// Positions text to a particular line
    void gotoLine();
            /// Increases text font
    void zoomIn();
            /// Decreases text font
    void zoomOut();

signals:
            /// signal emitted  when component status is changed
    void setInfo(const QString &str, int display_time);

protected slots:
    void expandBlock( Q3TextParagraph* );
    void collapseBlock( Q3TextParagraph* );
    void broadcastTargetNode(int);
    void highlightTargetNode(int);
    void removeHighlight(int, int);
    void showPopupMenu(const QPoint &point, int line, int col);

protected:
	// callback to handle toolbar clicks
	static void toolbarClicked(int);
    std::string unparseFileToString(SgFile *file, bool use_coords);

    //@{
          /// @name Event Handlers (overridden methods)

    virtual void eventCacheClear();
    virtual void eventCacheNewNodes(std::set<SgNode *> &nodes);
    virtual void receiveMsg(QRGroupWidget *emitter, QRMessage *msg);

    //@}

    void init(SgNode *root);
    void updateCacheView();
    void updateSourceView();
    void updateCacheViewButton();
    void doRepaint();
    void internalExpandBlock( Q3TextParagraph* );
    void internalCollapseBlock( Q3TextParagraph* );
    void lookupNode(SgNode *node);

    static int statusTimer;

protected:
    SgNode *m_rootNode;
    QRAstCoords *m_astCoords;
    bool m_isCacheView;
    bool m_isLookup;
    bool m_lookupWithHighlight;
    std::string m_sourceCode;
    std::string m_cacheSource;
    std::vector<QRCoordBox *> m_targetCoords;

    /* widgets */
    QR_EXPORT(QEditor, Editor);
    QR_EXPORT(LineNumberWidget,LineNumberWidget);
    QR_EXPORT(LevelWidget,LevelWidget);
    QR_EXPORT(Q3PopupMenu,Popup);
    QR_EXPORT(QStatusBar,Statusbar);
    QR_EXPORT(QRToolBar,Toolbar);
};

}

#endif
