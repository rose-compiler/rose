/***************************************************************************
 *   Copyright (C) 2005 by Jose G de Figueiredo Coutinho                   *
 *   jgfc@doc.ic.ac.uk                                                     *
 ***************************************************************************/

#include <rose.h>

#include <QRAttribute.h>
#include <QRException.h>
#include <rose.h>
#include <list>

#include <QPixmap>
#include <AstAttributeMechanism.h>


using namespace std;

namespace qrs {

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

bool QRAttribute::canModify() {
    return false;
}

bool QRAttribute::canEdit() {
    return false;
}

void QRAttribute::setAttribValues(QRItems &items) {

}

string QRAttribute::getNextValue(string current_value) {
    return current_value;
}

QPixmap *QRAttribute::getPixmap(std::string value) {
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
   }
}


void QRAttributeWidget::matchedAttributes(SgNode *node, map<string, AstAttribute *> &attributes) {
    attributes.clear();

    AstAttributeMechanism *attribute_mech = node->get_attributeMechanism();
    if (!attribute_mech) return;
    AstAttributeMechanism::AttributeIdentifiers ident = attribute_mech->getAttributeIdentifiers();

    for (AstAttributeMechanism::AttributeIdentifiers::iterator iter = ident.begin();
         iter != ident.end(); iter++)
    {
	bool find_attribute = m_attributes.find(*iter) != m_attributes.end();
	if (m_capture_all || find_attribute) {
	    attributes[*iter] = (*attribute_mech)[*iter];
	}
	if (m_capture_all && !find_attribute) {
	    m_attributes[*iter] = new QRAttribute(*iter);
	}
    }
}

QRAttribute *QRAttributeWidget::verifyAttribute(std::string attrib_name) {
    std::map<std::string, QRAttribute *>::iterator iter = m_attributes.find(attrib_name);
    if (iter != m_attributes.end())
	return iter->second;
    else
	return NULL;
}


QRAttribute *QRAttributeWidget::getAttribute(string attrib_name) {
    QRAttribute *attribute = verifyAttribute(attrib_name);
    eAssert(attribute, ("cannot find invalid attribute: %s", attrib_name.c_str()));
    return attribute;
}

void QRAttributeWidget::captureValues(std::string attrib_name,
                             QRAttribute::QRItems &items)
{
    QRAttribute *attribute = getAttribute(attrib_name);
    if (!attribute) return;
    attribute->setAttribValues(items);
}

bool QRAttributeWidget::isCaptureAll() { return m_capture_all; }
void QRAttributeWidget::setCaptureAll(bool capture_all) { m_capture_all = capture_all; }
bool QRAttributeWidget::isCaptureEnabled() { return m_capture_all || m_attributes.size() != 0; }

}
