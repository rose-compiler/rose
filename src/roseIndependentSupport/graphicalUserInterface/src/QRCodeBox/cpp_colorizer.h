#ifndef cpp_colorizer_h
#define cpp_colorizer_h

#include "qsourcecolorizer.h"
#include <qfont.h>

class CppColorizer: public QSourceColorizer{
public:
    CppColorizer( QEditor* );
    virtual ~CppColorizer();
    
    int computeLevel( QTextParagraph* parag, int startLevel );
	
private:
    void loadDynamicKeywords();
    
private:
    QMap<QString, int> m_dynamicKeywords;
};

#endif
