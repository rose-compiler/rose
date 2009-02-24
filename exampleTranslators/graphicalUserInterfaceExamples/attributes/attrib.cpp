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
#include <rose.h>
#include <config.h>

#include <qrose.h>
#include <sys/time.h>

class BooleanAttribute: public AstAttribute {
public:
    BooleanAttribute(): m_value(false) { }    
    string toString() { return (m_value? "yes" : "no"); }    
    void fromString(string str) {
       m_value = str == "yes";
   }
    
private:
       bool m_value;
};

class RandomFloatAttribute: public AstAttribute {
public:
    RandomFloatAttribute() {
	m_value = rand() / 100000;
    }
    string toString() { char buffer[100]; sprintf(buffer, "%.3f", m_value); return string(buffer); }    
    void fromString(string str) {
	m_value = atof(str.c_str());	
    }    
    int compare(RandomFloatAttribute *attrib) {
	if (m_value < attrib->m_value)
	    return -1;
	else if (m_value == attrib->m_value)
	    return 0;
	else 
	    return 1;
    }
    
private:
    float m_value;
};

template<class T>
class AstVisitor: public AstSimpleProcessing {
public:
    void select_node(int node_variant, string attrib_name) {
	m_node_variant = node_variant;
	m_attrib_name = attrib_name;
    }
    void visit(SgNode *node) {
	if (node->variantT() == m_node_variant) {
	    node->attribute.add(m_attrib_name, new T());
	}
    }
    
protected:
    int m_node_variant;
    string m_attrib_name;
};


int main(int argc, char *argv[])
{
    SgProject *project = frontend(argc, argv);
    QRGUI::init(argc,argv, project);
    
    QRTreeBox *tree = new QRTreeBox();

    printf("annotating AST...\n");
    AstVisitor<BooleanAttribute> visitor1;
    visitor1.select_node(V_SgFunctionDeclaration, "functions"); visitor1.traverseInputFiles(project, preorder);
    visitor1.select_node(V_SgClassDeclaration, "classes"); visitor1.traverseInputFiles(project, preorder);

    AstVisitor<RandomFloatAttribute> visitor2;
    visitor2.select_node(V_SgFunctionDeclaration, "performance");
    visitor2.traverseInputFiles(project, preorder);

    QRItems values;
    values["no"] = QRPixmap::get_pixmap(icons::func_decl); values["yes"] = QRPixmap::get_pixmap(icons::project);


 // tree->capture();

    tree->capture<RandomFloatAttribute>("performance", &RandomFloatAttribute::fromString, &RandomFloatAttribute::compare);
    tree->capture<BooleanAttribute>("functions", &BooleanAttribute::fromString);    
    tree->capture_values("functions", values);    
    tree->capture<BooleanAttribute>("classes", &BooleanAttribute::fromString);
   
    QRGUI::exec();
    //...code
    QRGUI::exec();
    
    
    
}
 
