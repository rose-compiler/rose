/***************************************************************************
    Jose Gabriel de Figueiredo Coutinho
    jgfc@doc.ic.ac.uk
    Class: QRToolBar
    Description:
 ***************************************************************************/

#ifndef QRTOOLBAR_H
#define QRTOOLBAR_H

#include <QRMacros.h>
#include <QRMain.h>
#include <QToolBar>
#include <vector>

namespace qrs {

class QRToolBar_p;

class QRToolBar: public QToolBar {

  Q_OBJECT

  friend class QRToolBar_p;

  public:
    QRToolBar(QROSE::Orientation orientation, bool showText = true,
              bool showPic = true, bool picBesidesText = true);
    virtual ~QRToolBar();

    int addButton(std::string caption, std::string icon_filename = "");
    int addButton(std::string caption, const char *icon_xpm[]);
    int addToggleButton(std::string caption, std::string icon_filename = "");
    int addToggleButton(std::string caption, const char *icon_xpm[]);
    void insertSeparator();

    void setCaption(int id, std::string caption);
    void setPicture(int id, std::string filename);
    void setPicture(int id, const char *xpm[]);
    void setEnabled(int id, bool enable);
    bool isEnabled(int id) const;
    void setChecked(int id, bool checked);
    bool isChecked(int id) const;
    bool isCheckable(int id) const;
    void setHidden(bool enable);
    bool isHidden() const;
    void setHidden(int id, bool enable);
    bool isHidden(int id) const;

    unsigned numButtons() const;

    QAction* getAction(int id) const;
    int getId(QAction *action) const;

  protected slots:
    virtual void clickEvent(int id);

  signals:
    void clicked(int id);

  protected:
     QRToolBar_p *_private;
};

}
#endif
