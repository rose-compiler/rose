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

#include <config.h>
#include <rose.h>
#include "QRAttribute.h"
#include <qpixmap.h>
#include <AstAttributeMechanism.h>
#include <list>

using namespace std;

QRAttribute::QRAttribute(string name) {
    m_name = name;
}

string QRAttribute::name() {
    return m_name;
}

void QRAttribute::fromString(AstAttribute *attrib, string str) { }

int QRAttribute::compare(AstAttribute *a1, AstAttribute *a2, bool ascending) {    
   if (!a1 && !a2) return 0;    
   if (!ascending) {
      if (a1 && !a2) return 1;
      if (!a1 && a2) return -1;
   } else {
      if (a1 && !a2) return -1;
      if (!a1 && a2) return 1;
   }
	    	
   return a1->toString().compare(a2->toString());    
}

bool QRAttribute::can_modify() {
    return false;
}

bool QRAttribute::can_edit() { 
    return false;
}

void QRAttribute::set_attribute_values(QRItems &items) { 
    
}

string QRAttribute::get_next_value(string current_value) {
    return current_value;
}

QPixmap *QRAttribute::get_pixmap(std::string value) {
    return NULL;
}


QRAttributeWidget::QRAttributeWidget() {
    m_capture_all = false;
}

QRAttributeWidget::~QRAttributeWidget() {
    map<std::string, QRAttribute *>::iterator iter = m_attributes.begin(); 
    while (iter != m_attributes.end()) {
	delete iter->second;
	iter++;
    }
}


void QRAttributeWidget::capture(string attrib_name) {
   if (attrib_name != "") {
       map<std::string, QRAttribute *>::iterator iter = m_attributes.find(attrib_name);
       if (iter != m_attributes.end()) {
	   delete iter->second;
       }
       m_attributes[attrib_name] = new QRAttribute(attrib_name);    
   } else {
       m_capture_all = true;
   }
}


void QRAttributeWidget::matched_attributes(SgNode *node, map<string, AstAttribute *> &attributes) {    
    attributes.clear();
    
    AstAttributeMechanism &attribute_mech = node->attribute;    
    AstAttributeMechanism::AttributeIdentifiers ident = attribute_mech.getAttributeIdentifiers();
    
    for (AstAttributeMechanism::AttributeIdentifiers::iterator iter = ident.begin();
         iter != ident.end(); iter++)
    {	
	bool find_attribute = m_attributes.find(*iter) != m_attributes.end(); 
	if (m_capture_all || find_attribute) {
	    attributes[*iter] = attribute_mech[*iter];
	}	
	if (m_capture_all && !find_attribute) {
	    m_attributes[*iter] = new QRAttribute(*iter);
	}    
    }
}

QRAttribute *QRAttributeWidget::verify_attribute(std::string attrib_name) {
    std::map<std::string, QRAttribute *>::iterator iter = m_attributes.find(attrib_name);
    if (iter != m_attributes.end())
	return iter->second;
    else
	return NULL;
}


QRAttribute *QRAttributeWidget::get_attribute(string attrib_name) {
    QRAttribute *attribute = verify_attribute(attrib_name);
    ASSERT(attribute);
    return attribute;
}

void QRAttributeWidget::capture_values(std::string attrib_name, QRItems &items) {
    QRAttribute *attribute = get_attribute(attrib_name);       
    attribute->set_attribute_values(items);  
}

    
bool QRAttributeWidget::is_capture_all() { return m_capture_all; }
void QRAttributeWidget::set_capture_all(bool capture_all) { m_capture_all = capture_all; }
bool QRAttributeWidget::is_capture_enabled() { return m_capture_all || m_attributes.size() != 0; }  
