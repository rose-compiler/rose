/***************************************************************************
 *   Copyright (C) 2005 by Jose G de Figueiredo Coutinho                   *
 *   jgfc@doc.ic.ac.uk                                                           *
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

#include "QRUnparser.h"

using namespace std;

namespace qrs {
QRAstCoords::QRAstCoords() {
    m_main_box = NULL;
}

QRAstCoords::~QRAstCoords() {
	for (map<SgNode *, QRCoordBox *>::iterator iter = m_node_coords.begin(); iter != m_node_coords.end(); iter++) {
		delete iter->second;
	}
}


bool QRAstCoords::push_coord_box(SgNode *node) {
    if (m_node_coords.find(node) != m_node_coords.end()) return false;

    QRCoordBox *box = new QRCoordBox();
    box->setNode(node);
    if (!m_box_stack.empty()) {
	   m_box_stack.top()->insertBox(box);
    } else {
	   m_main_box = box;
    }

    m_box_stack.push(box);
    m_node_coords[node] = box;

    return true;
}

void QRAstCoords::pop_coord_box(int line0, int col0, int line1, int col1) {
    QRCoordBox *box = m_box_stack.top(); m_box_stack.pop();
    box->setCoord(line0, col0, line1, col1);
}

std::map<SgNode *, QRCoordBox *>* QRAstCoords::nodeCoords() { return &m_node_coords; }

vector<QRCoordBox *> QRAstCoords::get_target_boxes(int line, int col) {
   vector<QRCoordBox *> m_target_boxes;
   if (m_main_box) {
	   get_target_boxes_aux(m_main_box, &m_target_boxes, line, col);
   }
   return m_target_boxes;
}


void QRAstCoords::get_target_boxes_aux(QRCoordBox *box,vector<QRCoordBox *> *target_boxes,  int line, int col) {
   target_boxes->push_back(box);
   const list<QRCoordBox *> *enclosed_boxes = box->children();
   if (enclosed_boxes->size() == 0) return;
   bool found_target_box = false;
   list<QRCoordBox *>::const_iterator iter = enclosed_boxes->begin();
   while (!found_target_box && iter != enclosed_boxes->end()) {
       QRCoordBox *new_box = *iter;
       if (new_box->isTarget(line, col)) {
  	      get_target_boxes_aux(new_box, target_boxes, line, col);
	      found_target_box = true;
       } else {
	      iter++;
       }
   }
}

class QRExprStmt: public Unparse_ExprStmt {
public:
	QRExprStmt(QRUnparser* unp, std::string fname): Unparse_ExprStmt(unp, fname) {
		_unparser = unp;
	}

	virtual void unparseStatement(SgStatement* stmt, SgUnparse_Info& info) {
		bool ret = _unparser->preUnparse(stmt);
		Unparse_ExprStmt::unparseStatement(stmt, info);
		if (ret) _unparser->postUnparse(stmt);
	}

	virtual void unparseExpression(SgExpression *expr, SgUnparse_Info &info) {
		bool ret = _unparser->preUnparse(expr);
		Unparse_ExprStmt::unparseExpression(expr, info);
		if (ret) _unparser->postUnparse(expr);
	}

protected:
	QRUnparser *_unparser;
};

/*
class QRTypes: public Unparse_Type {
public:

	QRTypes(QRUnparser* unp): Unparse_Type(unp) {
		_unparser = unp;
	}

	virtual void unparseType(SgType* type, SgUnparse_Info& info) {
		_unparser->preUnparse(type);
		Unparse_Type::unparseType(type, info);
		_unparser->postUnparse(type);
	}


protected:
	QRUnparser *_unparser;
};
*/


QRUnparser::QRUnparser( QRAstCoords *ast_coords, ostream* localStream, string filename, Unparser_Opt info,
	                    UnparseFormatHelp *h, UnparseDelegate* repl):
	Unparser(localStream, filename, info, h, repl)
{
	m_ast_coords = ast_coords;
    m_stream = (ostringstream *) localStream;
    m_format = &get_output_stream();

    string outputString;

    delete u_exprStmt;
    u_exprStmt = new QRExprStmt(this, filename);
    //delete u_type;
    //u_type = new QRTypes(this);
}

bool QRUnparser::preUnparse(SgNode *node) {
   bool ret = m_ast_coords->push_coord_box(node);
   if (ret) m_args.push(pair<int,int>( m_format->current_line()-1, m_format->current_col()));
   return ret;
}

void QRUnparser::postUnparse(SgNode *node) {
   if (m_ast_coords) {
      int l0 = m_args.top().first; int c0 = m_args.top().second; m_args.pop();
      int l1 = m_format->current_line()-1; int c1 = m_format->current_col();
      m_ast_coords->pop_coord_box(l0, c0, l1, c1);
   }
}

}
