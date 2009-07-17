/***************************************************************************
 *   Copyright (C) 2005 by Jose G de Figueiredo Coutinho                   *
 *   jgfc@doc.ic.ac.uk                                                          *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#ifndef QRUNPARSER_H
#define QRUNPARSER_H

//#include <rose.h>
#include <unparser.h>
#include <QRCoords.h>
#include <sstream>

#include <map>
#include <stack>
#include <list>
#include <vector>

class SgNode;

namespace qrs {

class QRAstCoords {
public:
    QRAstCoords();
    ~QRAstCoords();
    bool push_coord_box(SgNode *node);
    void pop_coord_box(int line0, int col0, int line1, int col1);

    std::map<SgNode *, QRCoordBox *>* nodeCoords();

    std::vector<QRCoordBox *> get_target_boxes(int line, int col);
    QRCoordBox *getMainBox() { return m_main_box; }

protected:
    void get_target_boxes_aux(QRCoordBox *box, std::vector<QRCoordBox *> *target_boxes,
    		int line, int col);


protected:
   QRCoordBox *m_main_box;
   std::map<SgNode *, QRCoordBox *>  m_node_coords;
   std::stack<QRCoordBox *> m_box_stack;
};


typedef void (*CustomUnparserFn) (SgNode *, std::string &);

class QRUnparser: public Unparser {
	friend class QRExprStmt;
	friend class QRTypes;

public:
  QRUnparser( QRAstCoords *ast_coords, std::ostream* localStream, std::string filename, Unparser_Opt info,
              UnparseFormatHelp *h = 0, UnparseDelegate* repl = 0);

protected:
   bool preUnparse(SgNode *node);
   void postUnparse(SgNode *node);

protected:
   QRAstCoords *m_ast_coords;
   std::ostringstream *m_stream;
   UnparseFormat  *m_format;
   std::stack <std::pair <int, int> > m_args;
};

}

#endif

