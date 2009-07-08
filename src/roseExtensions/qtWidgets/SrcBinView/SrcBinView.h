
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

class SrcBinView
    : public QWidget
{
    Q_OBJECT;

    public:
        SrcBinView( QWidget *parent = NULL );

        virtual ~SrcBinView();

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
