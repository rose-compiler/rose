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
#ifndef QRGUI_H
#define QRGUI_H

#include <string>
#include <list>
#include <map>
#include <qobject.h>
#include <QRCallback.h>


class QApplication;
class SgNode;
class QRGroup;
class QSplitter;
class QRPanelLayout;
class QRDialog;

class SgNode;
  
class QRGUI {
        
public:
    typedef enum {down, right} PanelType;
    static void init(int argc, char **argv, SgNode *root = NULL);
    static void destroy();
    
    static void set_root(SgNode *node);
    static SgNode* get_root();
       
    static QRDialog* dialog(unsigned id=0);    
    static QRPanelLayout* current_panel(QRPanelLayout *new_current_panel = NULL);
    static QSplitter* current_panel_frame();
    
    static void exec(unsigned id =0);
    
    static QApplication* app();
    
    static QRGroup *currentGroup();
    static bool autoGroup();
    static QRGroup* group(unsigned id = 0);
    
    static void panel_type(PanelType type);
    static PanelType panel_type();  
    static bool is_exec();
    
    static QRCallback *callback();
    
    static QObject* getWidget(const char *name, unsigned id = 0);
    template <class T>
	static T* getWidget(const char *name, unsigned id = 0) {
	   return (T *) getWidget(name, id);
        }
    
protected:
    static QApplication *m_app; 
    static SgNode *m_root;
    
    static PanelType m_panel_type;
    static QRPanelLayout *m_current_panel;
    static QRGroup *m_currentGroup;
    static bool m_autoGroup;
    
    static std::map<unsigned, QRGroup *> m_groups;
    static std::map<unsigned, QRDialog *> m_dialogs;
    
    static QRCallback *m_callback;
};



#endif
