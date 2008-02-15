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
#include "QRGui.h"
#include "QRGroup.h"
#include "QRLayout.h"
#include "QRPixmap.h"
#include <qapplication.h>
#include <qeventloop.h>

using namespace std;

QApplication* QRGUI::m_app = NULL;
SgNode* QRGUI::m_root = NULL;
QRGroup *QRGUI::m_currentGroup = NULL;
map<unsigned, QRGroup *> QRGUI::m_groups = map<unsigned, QRGroup *>();
map<unsigned, QRDialog *> QRGUI::m_dialogs = map<unsigned, QRDialog *>();
QRPanelLayout *QRGUI::m_current_panel = NULL;
bool QRGUI::m_autoGroup = true;
QRGUI::PanelType QRGUI::m_panel_type = QRGUI::down;
QRCallback* QRGUI::m_callback = NULL;

class QRGUI_destroy {

public:
    ~QRGUI_destroy() {
	QRGUI::destroy();
    }    
};  

static QRGUI_destroy gui_destroy;

void QRGUI::init(int argc, char **argv, SgNode *node) {
   m_app = new QApplication(argc, argv);    
   
   // create main dialog
   dialog(0);
   
   // create main group
   m_currentGroup = group(0);
   
   m_callback = new QRCallback();
      
   if (node) set_root(node);   
}

void QRGUI::destroy() {
 // DQ (8/13/2005): This output causes the test for zero output from simple use of ROSE translators to fail. so I have commented it out.
 // printf("destroying things...\n");
    QRPixmap::clear_pixmap_cache();
 // need to destroy everything else
    delete m_callback; 
}


void QRGUI::exec(unsigned id) {
    dialog(id)->show();
    m_app->exec();
}
    
QApplication* QRGUI::app() { return m_app; }


void QRGUI::set_root(SgNode *node) {
    m_root = node;
}

SgNode* QRGUI::get_root() {
    return m_root;
}

QRGroup *QRGUI::currentGroup() {
    return m_currentGroup;
}

bool QRGUI::autoGroup() {
    return m_autoGroup;
}

QRGroup* QRGUI::group(unsigned id) {
    QRGroup *grp;
    map<unsigned, QRGroup *>::iterator iter = m_groups.find(id);
    if (iter == m_groups.end()) {
	grp = new QRGroup(id);
    } else {
	grp = iter->second;
    }
    return grp;
}
	

QRDialog* QRGUI::dialog(unsigned id) {
    QRDialog *dlg;
    map<unsigned, QRDialog *>::iterator iter = m_dialogs.find(id);
    if (iter == m_dialogs.end()) {
        dlg = new QRDialog();
	m_dialogs[id] = dlg;
    } else {
	dlg = iter->second;
    }
    return dlg;
}

QRPanelLayout* QRGUI::current_panel(QRPanelLayout *new_current_panel) {
    if (new_current_panel) {
	m_current_panel = new_current_panel;
    }
    return m_current_panel;    
}

QSplitter* QRGUI::current_panel_frame() {
    return m_current_panel->frame();
}

void QRGUI::panel_type(PanelType type) {
    m_panel_type = type;
}

QRGUI::PanelType QRGUI::panel_type() {
    return m_panel_type;
}

bool QRGUI::is_exec() {
    return m_app && m_app->eventLoop() && m_app->eventLoop()->loopLevel();
}

QRCallback* QRGUI::callback() {
    return m_callback;
}

QObject* QRGUI::getWidget(const char *name, unsigned id) {
    map<unsigned, QRDialog *>::iterator iter = m_dialogs.find(id);
    if (iter != m_dialogs.end()) {
        QRDialog *dlg = iter->second;
        return dlg->child(name);
    }
    return NULL;
}
