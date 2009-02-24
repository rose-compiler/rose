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
#include <rose.h>
#include <config.h>

#include <qrose.h>

int  main(int argc, char **argv) {
    
    SgProject *project = frontend(argc,argv);
    
    // init generates dialog 0
    QRGUI::init(argc, argv, project);
    new QRCodeBox();
    
    // dialog 1
    QRGUI::panel_type(QRGUI::right);
    QRGUI::dialog(1);     
    QRGUI::panel_type(QRGUI::down);    
    new QRPanel();
       new QRCodeBox();
       new QRCodeBox();
       new QRCodeBox();
    QRGUI::current_panel(QRGUI::dialog(1));
       new QRCodeBox();
        
    // dialog 2  
    QRGUI::panel_type(QRGUI::down);
    QRGUI::dialog(2);
    new QRCodeBox();
    QRGUI::panel_type(QRGUI::right);    
    new QRPanel();
       new QRCodeBox();
       new QRCodeBox();
       new QRCodeBox();

    QRGUI::dialog(0)->show();
    QRGUI::dialog(1)->show();
    QRGUI::dialog(2)->show();
    
    QRGUI::exec();
    
}

