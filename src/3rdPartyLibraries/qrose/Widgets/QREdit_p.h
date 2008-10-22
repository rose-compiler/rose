#include <QREdit.h>
#include <QBoxLayout>
#include <QLabel>

#include <string>

namespace qrs {

class QREdit_p {
   friend class QREdit;
   
private:
   QREdit_p(QREdit *editor);
   void init(QREdit::Type type, const char *caption);
   void setText(std::string text);
   std::string getText() const;
   void clear();
   
   static void changeEvent(const char *str);
   static void changeEvent();
   
private:
   QREdit *_editor;
   QREdit::Type _type;
   QBoxLayout *_layout;
   QWidget *_widget;
   QLabel *_label;
};

}
