#ifndef QRUNPARSER_H
#define QRUNPARSER_H

#include <config.h>
#include <rose.h>
#include <unparser.h>
#include <QRCoords.h>
#include <stack>
#include <list>
#include <vector>
#include <set>
#include <map>

typedef map<SgNode *, QRCoordBox *> TypeNodeCoords;

class QRAstCoords {
public:
    QRAstCoords();
    void push_coord_box(SgNode *node);
    void pop_coord_box(int line0, int col0, int line1, int col1);
    
    TypeNodeCoords* node_coords();
    
    std::vector<QRCoordBox *> *get_target_boxes(int line, int col);   
    std::vector<QRCoordBox *> *get_stored_targets() { return &m_target_boxes; }
    bool node_has_multi_coord(SgNode *node);
    
protected:
    void get_target_boxes_aux(QRCoordBox *box, int line, int col);
    

protected:
   QRCoordBox *m_main_box;    
   TypeNodeCoords m_node_coords;   
   std::set<SgNode *> m_nodes_multi_coord;   
   std::stack<QRCoordBox *> m_box_stack;
   std::vector<QRCoordBox *> m_target_boxes;
};


class QRUnparser: public Unparser {
public:
  QRUnparser( QRAstCoords *ast_coords, ostream* localStream, char* filename, Unparser_Opt info, int lineNumberToUnparse,
            UnparseFormatHelp *h = 0, UnparseDelegate* repl = 0);

  void unparseStatement (SgStatement* stmt, SgUnparse_Info& info);
  void unparseExpression(SgExpression* expr, SgUnparse_Info& info);
  void unparseSymbol(SgSymbol* sym, SgUnparse_Info& info);
  void unparseType(SgType* type, SgUnparse_Info& info);
  
protected:
   QRAstCoords *m_ast_coords;
};


string QRUnparseToString (SgFile *astfile, QRAstCoords *ast_coords = NULL);

#endif

