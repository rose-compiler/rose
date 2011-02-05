/****************************************************************************
**
** Copyright (C) 2006-2009 fullmetalcoder <fullmetalcoder@hotmail.fr>
**
** This file is part of the Edyuk project <http://edyuk.org>
** 
** This file may be used under the terms of the GNU General Public License
** version 3 as published by the Free Software Foundation and appearing in the
** file GPL.txt included in the packaging of this file.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
****************************************************************************/

#include "qformatconfig.h"

/*!
        \file qformatconfig.cpp
        \brief Implementation of the QFormatConfig class.
        
        \see QFormatConfig
*/

#include "qformat.h"
#include "qformatscheme.h"

#include "qsimplecolorpicker.h"

#include <QHeaderView>
#include <QVBoxLayout>
#include <QMessageBox>
#include <QTableWidget>

/*!
        \ingroup dialogs
        @{
        
        \class QFormatConfig
        \brief A minimalistic, easy to embed, format settings widget.
        
*/

QFormatConfig::QFormatConfig(QWidget *w)
 : QWidget(w), m_currentScheme(0)
{
        setupUi(this);
        
        m_frame->hide();
        
        m_table->verticalHeader()->hide();
        
        m_table->horizontalHeaderItem(1)->setIcon(QIcon(":/bold.png"));
        m_table->horizontalHeaderItem(2)->setIcon(QIcon(":/italic.png"));
        m_table->horizontalHeaderItem(3)->setIcon(QIcon(":/underline.png"));
        //m_table->horizontalHeaderItem(4)->setIcon(QIcon(":/overline.png"));
        m_table->horizontalHeaderItem(5)->setIcon(QIcon(":/strikeout.png"));
        //m_table->horizontalHeaderItem(6)->setIcon(QIcon(":/waveUnderline.png"));
        m_table->horizontalHeaderItem(7)->setIcon(QIcon(":/textcolor.png"));
        m_table->horizontalHeaderItem(8)->setIcon(QIcon(":/fillcolor.png"));
        m_table->horizontalHeaderItem(9)->setIcon(QIcon(":/strokecolor.png"));
        
        connect(m_table, SIGNAL( itemSelectionChanged() ),
                        m_table, SLOT  ( clearSelection() ) );
        
}

/*!
        \brief run-time translation entry point
*/
void QFormatConfig::retranslate()
{
        retranslateUi(this);
}

/*!
        \brief 
*/
bool QFormatConfig::hasUnsavedChanges() const
{
        return modifiedFormats().count();
}

/*!
        \brief
*/
QList<QFormatScheme*> QFormatConfig::schemes() const
{
        return m_schemes;
}

/*!
        \brief
*/
void QFormatConfig::addScheme(const QString& name, QFormatScheme *scheme)
{
        m_schemes << scheme;
        
        m_selector->addItem(name);
        
        if ( m_schemes.count() > 1 && m_schemes.contains(m_currentScheme) )
        {
                // show the scheme selector
                m_frame->show();
        }
        
        if ( !m_currentScheme )
                setCurrentScheme(scheme);
}

/*!
        \brief
*/
void QFormatConfig::removeScheme(QFormatScheme *s)
{
        if ( m_currentScheme == s )
        {
                m_currentScheme = 0;
        }
        
        for ( int i = 0; i < m_schemes.count(); ++i )
        {
                if ( m_schemes.at(i) == s )
                {
                        m_selector->removeItem(i);
                        m_schemes.removeAt(i);
                }
        }
        
        if ( m_schemes.count() <= 1 )
        {
                // hide the scheme selector
                m_frame->hide();
        }
}

/*!
        \brief
*/
void QFormatConfig::setCurrentScheme(QFormatScheme *s)
{
        int idx = m_schemes.indexOf(s);
        
        if ( idx != -1 )
        {
                m_currentScheme = s;
                
                // update table widget
                cancel();
        } else {
                m_selector->setCurrentIndex(idx);
        }
}

/*!
        \brief Apply changes
*/
void QFormatConfig::apply()
{
        if ( m_currentScheme )
        {
                const int n = m_currentScheme->formatCount();
                
                m_table->setRowCount(n);
                
                for ( int i = 0 ; i < n; ++i )
                {
                        QString fid = m_currentScheme->id(i);
                        QFormat& fmt = m_currentScheme->formatRef(i);
                        
                        QTableWidgetItem *item;
                        
                        item = m_table->item(i, 1);
                        fmt.weight = item->checkState() == Qt::Checked ? QFont::Bold : QFont::Normal;
                        
                        item = m_table->item(i, 2);
                        fmt.italic = item->checkState() == Qt::Checked;
                        
                        item = m_table->item(i, 3);
                        fmt.underline = item->checkState() == Qt::Checked;
                        
                        item = m_table->item(i, 4);
                        fmt.overline = item->checkState() == Qt::Checked;
                        
                        item = m_table->item(i, 5);
                        fmt.strikeout = item->checkState() == Qt::Checked;
                        
                        item = m_table->item(i, 6);
                        fmt.waveUnderline = item->checkState() == Qt::Checked;
                        
                        QSimpleColorPicker *cp;
                        
                        cp = qobject_cast<QSimpleColorPicker*>(m_table->cellWidget(i, 7));
                        if ( cp )
                                fmt.foreground = cp->color();
                        
                        cp = qobject_cast<QSimpleColorPicker*>(m_table->cellWidget(i, 8));
                        if ( cp )
                                fmt.background = cp->color();
                        
                        cp = qobject_cast<QSimpleColorPicker*>(m_table->cellWidget(i, 9));
                        if ( cp )
                                fmt.linescolor = cp->color();
                        
                }
                
                // TODO : save scheme and update editors
                
                // this will only save schemes loaded from an existing file
                m_currentScheme->save();
        }
}

/*!
        \brief Reset the subcontrols to reflect the current settings
        
        The name can be a bit misleading at first, it has been chosen
        because it directly maps to the effect a "cancel" button would
        have on the widget
*/
void QFormatConfig::cancel()
{
        m_table->clearContents();
        
        if ( m_currentScheme )
        {
                const int n = m_currentScheme->formatCount();
                
                m_table->setRowCount(n);
                
                for ( int i = 0 ; i < n; ++i )
                {
                        QString fid = m_currentScheme->id(i);
                        const QFormat& fmt = m_currentScheme->formatRef(i);
                        
                        QTableWidgetItem *item;
                        
                        item = new QTableWidgetItem(fid);
                        item->setFlags(Qt::ItemIsEnabled);
                        m_table->setItem(i, 0, item);
                        
                        item = new QTableWidgetItem;
                        item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsUserCheckable);
                        item->setCheckState(fmt.weight == QFont::Bold ? Qt::Checked : Qt::Unchecked);
                        item->setToolTip(tr("Bold"));
                        m_table->setItem(i, 1, item);
                        
                        item = new QTableWidgetItem;
                        item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsUserCheckable);
                        item->setCheckState(fmt.italic ? Qt::Checked : Qt::Unchecked);
                        item->setToolTip(tr("Italic"));
                        m_table->setItem(i, 2, item);
                        
                        item = new QTableWidgetItem;
                        item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsUserCheckable);
                        item->setCheckState(fmt.underline ? Qt::Checked : Qt::Unchecked);
                        item->setToolTip(tr("Underline"));
                        m_table->setItem(i, 3, item);
                        
                        item = new QTableWidgetItem;
                        item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsUserCheckable);
                        item->setCheckState(fmt.overline ? Qt::Checked : Qt::Unchecked);
                        item->setToolTip(tr("Overline"));
                        m_table->setItem(i, 4, item);
                        
                        item = new QTableWidgetItem;
                        item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsUserCheckable);
                        item->setCheckState(fmt.strikeout ? Qt::Checked : Qt::Unchecked);
                        item->setToolTip(tr("Strikeout"));
                        m_table->setItem(i, 5, item);
                        
                        item = new QTableWidgetItem;
                        item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsUserCheckable);
                        item->setCheckState(fmt.waveUnderline ? Qt::Checked : Qt::Unchecked);
                        item->setToolTip(tr("Wave underline"));
                        m_table->setItem(i, 6, item);
                        
                        m_table->setCellWidget(i, 7, new QSimpleColorPicker(fmt.foreground));
                        m_table->cellWidget(i, 7)->setToolTip(tr("Text color (aka foreground)"));
                        //m_table->cellWidget(i, 7)->setMaximumSize(22, 22);
                        
                        m_table->setCellWidget(i, 8, new QSimpleColorPicker(fmt.background));
                        m_table->cellWidget(i, 8)->setToolTip(tr("Background color"));
                        //m_table->cellWidget(i, 8)->setMaximumSize(22, 22);
                        
                        m_table->setCellWidget(i, 9, new QSimpleColorPicker(fmt.linescolor));
                        m_table->cellWidget(i, 9)->setToolTip(tr("Lines color (used by all lines formatting : underline, overline, ...)"));
                        //m_table->cellWidget(i, 9)->setMaximumSize(22, 22);
                }
        }
        
        m_table->resizeColumnsToContents();
}

/*!
        \brief Restore default values for all subcontrols
        
        \note The widgets are changed but these changes are NOT applied.
*/
void QFormatConfig::restore()
{
        // restoring what? this is only provided for API consistency and in case
        // (very unlikely to ever happen) design changes make restore() a sensible
        // thing to do on format schemes
}

QList<int> QFormatConfig::modifiedFormats() const
{
        QList<int> hasModif;
        
        if ( m_currentScheme )
        {
                const int n = m_currentScheme->formatCount();
                
                m_table->setRowCount(n);
                
                for ( int i = 0 ; i < n; ++i )
                {
                        QString fid = m_currentScheme->id(i);
                        QFormat& fmt = m_currentScheme->formatRef(i);
                        
                        QTableWidgetItem *item;
                        
                        item = m_table->item(i, 1);
                        if ( fmt.weight != (item->checkState() == Qt::Checked ? QFont::Bold : QFont::Normal) )
                        {
                                hasModif << i;
                                continue;
                        }
                        
                        item = m_table->item(i, 2);
                        if ( fmt.italic != (item->checkState() == Qt::Checked) )
                        {
                                hasModif << i;
                                continue;
                        }
                        
                        item = m_table->item(i, 3);
                        if ( fmt.underline != (item->checkState() == Qt::Checked) )
                        {
                                hasModif << i;
                                continue;
                        }
                        
                        item = m_table->item(i, 4);
                        if ( fmt.overline != (item->checkState() == Qt::Checked) )
                        {
                                hasModif << i;
                                continue;
                        }
                        
                        item = m_table->item(i, 5);
                        if ( fmt.strikeout != (item->checkState() == Qt::Checked) )
                        {
                                hasModif << i;
                                continue;
                        }
                        
                        item = m_table->item(i, 6);
                        if ( fmt.waveUnderline != (item->checkState() == Qt::Checked) )
                        {
                                hasModif << i;
                                continue;
                        }
                        
                        QSimpleColorPicker *cp;
                        
                        cp = qobject_cast<QSimpleColorPicker*>(m_table->cellWidget(i, 7));
                        if ( cp && fmt.foreground != cp->color() )
                        {
                                hasModif << i;
                                continue;
                        }
                        
                        cp = qobject_cast<QSimpleColorPicker*>(m_table->cellWidget(i, 8));
                        if ( cp && fmt.background != cp->color() )
                        {
                                hasModif << i;
                                continue;
                        }
                        
                        cp = qobject_cast<QSimpleColorPicker*>(m_table->cellWidget(i, 9));
                        if ( cp && fmt.linescolor != cp->color() )
                        {
                                hasModif << i;
                                continue;
                        }
                }
        }
        
        return hasModif;
}

void QFormatConfig::on_m_selector_currentIndexChanged(int idx)
{
        QList<int> hasModif = modifiedFormats();
        
        if ( hasModif.count() )
        {
                // TODO : provide custom widget to allow user to select which items should be saved?
                int ret = QMessageBox::warning(
                                                                        0, 
                                                                        tr("Unsaved changes"),
                                                                        tr("There are unsaved changes in this format scheme.\nDo you want them to be saved?"),
                                                                        QMessageBox::Save | QMessageBox::Discard
                                                                );
                
                if ( ret == QMessageBox::Save )
                {
                        apply();
                }
        }
        
        m_currentScheme = idx >= 0 && idx < m_schemes.count() ? m_schemes.at(idx) : 0;
        
        cancel();
}

/*! @} */
