
#ifndef ASM_INSTRUCTION_BAR
#define ASM_INSTRUCTION_BAR

#include <vector>
#include <QList>
#include <QVector>
#include <QGraphicsView>
#include <QGraphicsScene>

class SgNode;
class QMouseEvent;
class QWheelEvent;
class QGraphicsRectItem;
class QGraphicsPolygonItem;

class AsmInstructionsBar
   : public QGraphicsView
{
   Q_OBJECT

   public:
      AsmInstructionsBar( QWidget *parent = NULL );
      virtual ~AsmInstructionsBar();

      int getNumberOfBlocks();

   public slots:
      //slots to set new nodes:
      // if the given node is a binary node, everything is fine
      // if it is source, the associated binary node will be used
      // Use the AsmToSourceMapper to link binary nodes with source nodes

      // set new root node.
      void setRoot( SgNode *node );

      // set current active node
      void setNode( SgNode *node );

      // set current active node by id, i.e. the position in the array of
      // Assembler Blocks
      void setNode( int id );

   signals:
      // emit current active node
      void clicked( SgNode *node );
      // emit current active node id, again the id is the position in the
      // array of Assembler Blocks
      void clicked( int id );

   protected:

      void setIndicator( );

      //virtual void resizeEvent(QResizeEvent *ev);
      virtual void mousePressEvent( QMouseEvent *e );
      virtual void mouseReleaseEvent( QMouseEvent *e );
      virtual void mouseMoveEvent( QMouseEvent *e );
      virtual void wheelEvent( QWheelEvent *e );
      virtual void dropEvent( QDropEvent * ev);
      virtual void dragMoveEvent( QDragMoveEvent * ev);
      virtual void dragEnterEvent( QDragEnterEvent * ev);

      QGraphicsScene scene;

      QVector<QGraphicsRectItem *> blockList;
      QGraphicsPolygonItem *indicatorBottom;
      QGraphicsRectItem    *indicatorMiddle;
      QGraphicsPolygonItem *indicatorTop;

      QGraphicsRectItem *currentBlock;
   
   private:
      void updatePosition( const QPoint& pos );

      SgNode *root;

      const uint64_t base_width;
      const float    height;

      bool pressed;
};

#endif
