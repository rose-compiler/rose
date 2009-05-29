
#ifndef ASM_INSTRUCTION_BAR
#define ASM_INSTRUCTION_BAR

#include <QGraphicsView>
#include <QGraphicsScene>

class SgNode;

class AsmInstructionsBar
   : public QGraphicsView
{
   Q_OBJECT

   public:
      AsmInstructionsBar( QWidget *parent = NULL );
      virtual ~AsmInstructionsBar();

      void setNode( SgNode *node );

   protected:
      QGraphicsScene scene;
};

#endif
