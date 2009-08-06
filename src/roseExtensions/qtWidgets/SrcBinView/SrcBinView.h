
#ifndef SrcBinView_H
#define SrcBinView_H

#include <utility>
#include <map>

#include <QWidget>

#include <map>

namespace Ui { class SrcBinView; }

class SgNode;
class SgProject;
class SgNode;
class SgBinaryFile;
class SgSourceFile;

class QDropEvent;
class QDragMoveEvent;
class QDragEnterEvent;

struct SourceRange
{
    std::pair<int, int> start;
    std::pair<int, int> end;
    /*int line_start;
    int col_start;

    int line_end;
    int col_end;*/

    bool operator<( const SourceRange& other ) const
    {
        //return line_start < other.line_start;
        return start < other.start;
    }
};

/**
 * \brief Displays Source and Binary  side-by-side
 *
 *  - Relies on AsmToSourceMapper annotations
 *  - has own (hacky) mechanism to map from a SourceCode location (cursor-location) to the according SgNode,
 *    and from there with AsmToSourceMapper annotation to the binary
 *    TODO use mechanism here which is used in QRSourceBox
 */
class SrcBinView : public QWidget
{
    Q_OBJECT;

    public:
        SrcBinView( QWidget *parent = NULL );

        virtual ~SrcBinView();
        
        virtual bool eventFilter( QObject *object, QEvent *event );
    
    protected:
        virtual void dropEvent( QDropEvent * ev);
        virtual void dragMoveEvent( QDragMoveEvent * ev);
        virtual void dragEnterEvent( QDragEnterEvent * ev);

    private slots:
        void setSourceNode( SgNode *node, bool setCodeEdit = true );
        void setBinaryNode( SgNode *node );
        void getCursorPosition();

    private:
        Ui::SrcBinView *srcBinView;

        SgBinaryFile *binFile;
        SgSourceFile *srcFile;

        void fillMap( SgNode *node );

        std::multimap<SourceRange, SgNode *> lineColToSgNode;
};

#endif
