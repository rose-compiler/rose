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

#ifndef QRATTRIBUTE_H
#define QRATTRIBUTE_H

#include <string>
#include <map>
#include <qpixmap.h>

class AstAttribute;

typedef std::map<std::string, QPixmap *> QRItems;

class QRAttribute {  
public:    
    QRAttribute(std::string name);
    virtual void fromString(AstAttribute *attrib, std::string str);
    virtual int compare(AstAttribute *a1, AstAttribute *a2, bool ascending);    
    virtual bool can_modify();  
    virtual bool can_edit();
    virtual void set_attribute_values(QRItems &items);   
    virtual std::string get_next_value(std::string current_value);
    virtual QPixmap *get_pixmap(std::string value);
    
    std::string name();
    
protected:
    std::string m_name;    
};   

template <class T>
class QRAttribSpec: public QRAttribute {
public:
    QRAttribSpec(std::string name, void (T::*fromString)(std::string), int (T::*compare)(T *)):
	QRAttribute(name), m_fromString(fromString), m_compare(compare), m_values(NULL) { }
    
    virtual void fromString(AstAttribute *attrib, std::string str) {
	if (m_fromString) {
	    (((T *) attrib)->*m_fromString)(str);
	}
    }
    
    virtual int compare(AstAttribute *a1, AstAttribute *a2, bool ascending) {
	if (m_compare == NULL || !a1 || !a2) {
	    return QRAttribute::compare(a1, a2, ascending);
	} else {
	    return ((((T *) a1)->*m_compare) ((T *) a2));
	}	
    }
    
    virtual bool can_modify() {
	return m_fromString != NULL;
    }
    
    virtual bool can_edit() {
	return can_modify() && (!m_values || (m_values->size() == 0));
    }
    
    virtual void set_attribute_values(QRItems &items) {
	if (m_values) delete m_values;
	m_values = new QRItems(items);
	    
    }
    
    virtual std::string get_next_value(std::string current_value) {
	if (!m_values || m_values->size() == 0) {
	    return QRAttribute::get_next_value(current_value);
	} else {
	    QRItems::iterator iter = m_values->find(current_value);
	    iter++;
	    if (iter == m_values->end())
		return m_values->begin()->first;
	    else
		return iter->first;
	}
    }
    
    virtual QPixmap *get_pixmap(std::string value) {
	if (!m_values) return NULL;
	QRItems::iterator iter = m_values->find(value);
	if (iter == m_values->end()) {
	    return NULL;
	} else {
	    return iter->second;
	}
    }
        
protected:
    void (T::*m_fromString) (std::string);
    int  (T::*m_compare) (T*);
    QRItems *m_values;    
};
		

class QRAttributeWidget {
public:
    QRAttributeWidget();
    ~QRAttributeWidget();
    
    void capture(std::string attrib_name = "");     
    
    void capture_values(std::string attrib_name, QRItems &items);
    	
   template <class T>
	void capture(std::string attrib_name, void (T::*fromString)(std::string)) {
	   capture_attribute<T>(attrib_name, fromString, NULL);
        }
    
    template<class T>
	void capture(std::string attrib_name, int (T::*compare)(T *)) {
	   capture_attribute<T>(attrib_name, NULL, compare);
        }
    template<class T>
	void capture(std::string attrib_name, void (T::*fromString)(std::string), int (T::*compare)(T *)) {
	   capture_attribute<T>(attrib_name, fromString, compare);
        }
       
    bool is_capture_all();
    void set_capture_all(bool capture_all);
    bool is_capture_enabled();
    
    
    QRAttribute *verify_attribute(std::string attrib_name);        
    QRAttribute *get_attribute(std::string attrib_name);    
    
    void matched_attributes(SgNode *node, std::map<std::string, AstAttribute *> &attributes);    
    
protected:    
    template <class T>
	void capture_attribute(std::string attrib_name, void (T::*fromString)(std::string), int (T::*compare)(T *)) {
	   std::map<std::string, QRAttribute *>::iterator iter = m_attributes.find(attrib_name);
	   if (iter != m_attributes.end()) {
               delete iter->second;
	   }
	   m_attributes[attrib_name] = new QRAttribSpec<T>(attrib_name, fromString, compare);	
        } 
	   
protected:    
    std::map<std::string, QRAttribute *> m_attributes;
    bool m_capture_all;
};

#endif



