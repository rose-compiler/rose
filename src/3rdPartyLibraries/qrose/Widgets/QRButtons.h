/***************************************************************************
 *   Jose Gabriel de Figueiredo Coutinho                                   *
 *   jgfc@doc.ic.ac.uk                                                     *
 *   QRose GUI Library                                                     * 
 *   Class: QRButtons                                                      *
 ***************************************************************************/

#ifndef QRBUTTONS_H
#define QRBUTTONS_H

#include <QRMain.h>
#include <QRPanel.h>

#include <QAbstractButton>
#include <QPixmap>

#include <vector>
namespace qrs {

// this is a specialized panel where you can add
// buttons.
class QRButtons: public QRPanel {

   Q_OBJECT
			
	friend class QRButtons_p;

public:
   typedef enum Type { 
      Normal,
      Toggle,
      Check,
      Radio
   };

	QRButtons(std::string caption = "");
	QRButtons(QROSE::Orientation orientation, std::string caption = "");
		
	void addButtons(int numButtons, Type type);
	unsigned numButtons() const;
   
	QAbstractButton* operator [](int id) const;
	int getId(QAbstractButton *button);
   Type getType(int id) const;
	
	void setCaption(const char *caption0, ...);
	void setCaption(int id, const char *fmt_caption, ...);
   	
	void setPicture(int id, const char *filename);
   void setPicture(int id, const char *xpm[]);
   
	void setBtnChecked(int id, bool check);
	bool isBtnChecked(int id) const;
   bool isBtnCheckable(int id) const;

   void setBtnEnabled(int id, bool enable);
   bool isBtnEnabled(int id) const;
   
	
protected slots:
	virtual void clickEvent(int id);
	
signals:
	void clicked(int id);
		
protected:
   std::vector<QAbstractButton *> _buttons; 
	std::vector<Type> _button_types;
   bool _has_radio_btns;
};

}
#endif


