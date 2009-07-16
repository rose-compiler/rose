/***************************************************************************
 *   Copyright (C) 2005 by Jose G de Figueiredo Coutinho                   *
 *   jgfc@doc.ic.ac.uk                                                     *
 *   Class: QRAttribute, QRAttribSpec<T>, QRAttributeWidget                *
 ***************************************************************************/

#ifndef QRATTRIBUTE_H
#define QRATTRIBUTE_H

#include <QPixmap>
#include <map>
#include <string>

class AstAttribute;
class SgNode;

namespace qrs {

/**
* \ingroup qrose_comps
*/

/// Class that encapsulates IR attributes with the same name
class QRAttribute {
public:
    /// Map type that associates string values with icons
    typedef std::map<std::string, QPixmap *> QRItems;

    //@{
          /// @name Initialization

          /// Creates an QRAttribute with the name of AstAttributes that it encapsulates
    QRAttribute(std::string name);
    //@}

     //@{
          /// @name Properties

          /// Returns attribute name
    std::string name();
          /// Returns whether attribute is read-only
    virtual bool canModify();
          /// Returns whether attribute can be modified to arbitrary values
    virtual bool canEdit();
          /// Returns a pixmap (if available) for a value
    virtual QPixmap *getPixmap(std::string value);
          /// Returns the next value
    virtual std::string getNextValue(std::string current_value);
      //@}

     //@{
          /// @name Actions

          /// Sets values (string+pixmap) for this attribute
    virtual void setAttribValues(QRItems &items);
          /// Converts a string to an IR attribute (AstAtribute)
    virtual void fromString(AstAttribute *attrib, std::string str);
          /// How IR attributes (AstAttribute) are sorted
    virtual int compare(AstAttribute *a1, AstAttribute *a2, bool ascending);
     //@}

     virtual ~QRAttribute() { }

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

    virtual bool canModify() {
	return m_fromString != NULL;
    }

    virtual bool canEdit() {
	return canModify() && (!m_values || (m_values->size() == 0));
    }

    virtual void setAttribValues(QRItems &items) {
	if (m_values) delete m_values;
	m_values = new QRItems(items);

    }

    virtual std::string getNextValue(std::string current_value) {
	if (!m_values || m_values->size() == 0) {
	    return QRAttribute::getNextValue(current_value);
	} else {
	    QRItems::iterator iter = m_values->find(current_value);
	    iter++;
	    if (iter == m_values->end())
		return m_values->begin()->first;
	    else
		return iter->first;
	}
    }

    virtual QPixmap *getPixmap(std::string value) {
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

/**
* \ingroup qrose_comps
*/

/// ROSE component that manages ROSE IR attributes (AstAttribute)
class QRAttributeWidget {
public:
    QRAttributeWidget();
    ~QRAttributeWidget();

    //@{
           /// @name Attribute capture

           ///  Captures AstAtribute with name @a attrib_name
    void capture(std::string attrib_name);
    template <class T>
	void capture(std::string attrib_name, void (T::*fromString)(std::string)) {
	   captureAttribute<T>(attrib_name, fromString, NULL);
        }
           /// Same as above, but provides a sort method that compares two AstAttrib objects
    template<class T>
	void capture(std::string attrib_name, int (T::*compare)(T *)) {
	   captureAttribute<T>(attrib_name, NULL, compare);
        }
           /// Same as above, but provides a method to modify/edit an AstAttrib
    template<class T>
	void capture(std::string attrib_name, void (T::*fromString)(std::string), int (T::*compare)(T *)) {
	   captureAttribute<T>(attrib_name, fromString, compare);
        }
          /// Provides valid values for AstAttribute with name @a name attrib_name.
    void captureValues(std::string attrib_name, QRAttribute::QRItems &items);

          /// Returns true if capture is enabled
    bool isCaptureEnabled();
          /// Returns true if all attributes should be captured
    bool isCaptureAll();
          /// Sets widget to capture all attributes
    void setCaptureAll(bool capture_all);
    //@}

    //@{
           /// @name Attribute lookup

           /// Returns QRAttribute object corresponding to name @a attrib_name, and NULL if it does not exist
    QRAttribute *verifyAttribute(std::string attrib_name);
           /// Same as above, except that it fails (assert) if it does not exist.
    QRAttribute *getAttribute(std::string attrib_name);
           /// Extracts all IR attributes from an IR node
    void matchedAttributes(SgNode *node,
                           std::map<std::string, AstAttribute *> &attributes);
    //@}

protected:
    template <class T>
	void captureAttribute(std::string attrib_name, void (T::*fromString)(std::string), int (T::*compare)(T *)) {
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

}

#endif



