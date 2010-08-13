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

#include "qeditconfig.h"
#include "ui_editconfig.h"
/*!
	\file qeditconfig.cpp
	\brief Implementation of the QEditConfig class.

	\see QEditConfig
*/

#include "qeditor.h"
#include "qdocument.h"
#include "qdocument_p.h"

#include <QDebug>
#include <QTextCodec>

/*!
	\ingroup dialogs
	@{

	\class QEditConfig
	\brief A minimalistic, easy to embed, settings widget.

*/

QEditConfig::QEditConfig(QWidget *w)
 : QWidget(w), m_direct(false)
{
        ui = new Ui::EditorConfig();
	ui->setupUi(this);

	QStringList l;
	QList<QByteArray> ens = QTextCodec::availableCodecs();

	foreach ( QByteArray b, ens )
		l << QString::fromLatin1(b);

	ui->cbEncoding->clear();
	ui->cbEncoding->addItems(l);

	restore();
}

QEditConfig::~QEditConfig()
{
    delete ui;
}

/*!
	\brief run-time translation entry point
*/
void QEditConfig::retranslate()
{
	ui->retranslateUi(this);
}

/*!
	\brief
*/
bool QEditConfig::hasUnsavedChanges() const
{
	if ( m_direct )
		return false;

	QFont font = ui->cbFont->currentFont();
	//font.setPointSize(spnFontSize->value());

	const QFont& docFont = QDocument::font();

	if ( font.family() != docFont.family() || ui->spnFontSize->value() != docFont.pointSize() )
	{
		//qDebug("font!");
		return true;
	}

	if ( ui->spnTabWidth->value() != QDocument::tabStop() )
	{
		//qDebug("tab stop!");
		return true;
	}

	QDocument::LineEnding le = QDocument::defaultLineEnding();

	if ( ui->chkDetectLE->isChecked() )
	{
		if ( le != QDocument::Conservative )
		{
			//qDebug("conservative line endings! : %i", le);
			return true;
		}
	} else {
		if ( le != QDocument::LineEnding(ui->cbLineEndings->currentIndex() + 1) )
		{
			//qDebug("line endings!");
			return true;
		}
	}

	QDocument::WhiteSpaceMode ws = QDocument::ShowNone;

	if ( ui->chkShowLeadingWhitespace->isChecked() )
		ws |= QDocument::ShowLeading;

	if ( ui->chkShowTrailingWhitespace->isChecked() )
		ws |= QDocument::ShowTrailing;

	if ( ui->chkShowTabsInText->isChecked() )
		ws |= QDocument::ShowTabs;

	if ( ws != QDocument::showSpaces() )
	{
		//qDebug("spaces!");
		return true;
	}

	QTextCodec *c = QEditor::defaultCodec();

	if ( ui->cbEncoding->currentText() == "System" )
	{
		if ( c && c->name() != "System" )
		{
			//qDebug("system codec!");
			return true;
		}
	} else {
		if ( !c || c->name() != ui->cbEncoding->currentText().toLatin1() )
		{
			//qDebug("codec!");
			return true;
		}
	}

	int flags = QEditor::defaultFlags();

	if ( ui->chkReplaceTabs->isChecked() )
		flags |= QEditor::ReplaceTabs;
	else
		flags &= ~QEditor::ReplaceTabs;

	if ( ui->chkAutoRemoveTrailingWhitespace->isChecked() )
		flags |= QEditor::RemoveTrailing;
	else
		flags &= ~QEditor::RemoveTrailing;

	if ( ui->chkPreserveTrailingIndent->isChecked() )
		flags |= QEditor::PreserveTrailingIndent;
	else
		flags &= ~QEditor::PreserveTrailingIndent;

	if ( flags != QEditor::defaultFlags() )
	{
		//qDebug("flags!");
		return true;
	}

	return false;
}

/*!
	\return whether user changes are immediately applied
*/
bool QEditConfig::applyImmediately() const
{
	return m_direct;
}

/*!
	\brief Set whether user changes are immediately applied
*/
void QEditConfig::setApplyImmediately(bool y)
{
	m_direct = y;
}

/*!
	\brief Apply changes
*/
void QEditConfig::apply()
{
	QFont font = ui->cbFont->currentFont();
	font.setPointSize(ui->spnFontSize->value());

	QDocument::setFont(font);
	QDocument::setTabStop(ui->spnTabWidth->value());

	if ( ui->chkDetectLE->isChecked() )
		QDocument::setDefaultLineEnding(QDocument::Conservative);
	else
		QDocument::setDefaultLineEnding(QDocument::LineEnding(ui->cbLineEndings->currentIndex() + 1));

	QDocument::WhiteSpaceMode ws = QDocument::ShowNone;

	if ( ui->chkShowLeadingWhitespace->isChecked() )
		ws |= QDocument::ShowLeading;

	if ( ui->chkShowTrailingWhitespace->isChecked() )
		ws |= QDocument::ShowTrailing;

	if ( ui->chkShowTabsInText->isChecked() )
		ws |= QDocument::ShowTabs;

	QDocument::setShowSpaces(ws);

	if ( ui->cbEncoding->currentText() == "System" )
		QEditor::setDefaultCodec(0, QEditor::UpdateAll);
	else
		QEditor::setDefaultCodec(ui->cbEncoding->currentText().toLatin1(), QEditor::UpdateAll);

	int flags = QEditor::defaultFlags();

	if ( ui->chkReplaceTabs->isChecked() )
		flags |= QEditor::ReplaceTabs;
	else
		flags &= ~QEditor::ReplaceTabs;

	if ( ui->chkAutoRemoveTrailingWhitespace->isChecked() )
		flags |= QEditor::RemoveTrailing;
	else
		flags &= ~QEditor::RemoveTrailing;

	if ( ui->chkPreserveTrailingIndent->isChecked() )
		flags |= QEditor::PreserveTrailingIndent;
	else
		flags &= ~QEditor::PreserveTrailingIndent;

	QEditor::setDefaultFlags(flags);
}

/*!
	\brief Reset the subcontrols to reflect the current settings

	The name can be a bit misleading at first, it has been chosen
	because it directly maps to the effect a "cancel" button would
	have on the widget
*/
void QEditConfig::cancel()
{
	// reload the current config

	bool oldDir = m_direct;

	m_direct = false;

	ui->cbFont->setFont(QDocument::font());
	ui->spnFontSize->setValue(QDocument::font().pointSize());

	ui->spnTabWidth->setValue(QDocument::tabStop());

	QDocument::WhiteSpaceMode ws = QDocument::showSpaces();
	ui->chkShowTabsInText->setChecked(ws & QDocument::ShowTabs);
	ui->chkShowLeadingWhitespace->setChecked(ws & QDocument::ShowLeading);
	ui->chkShowTrailingWhitespace->setChecked(ws & QDocument::ShowTrailing);

	QDocument::LineEnding le = QDocument::defaultLineEnding();
	ui->chkDetectLE->setChecked(le == QDocument::Conservative);
	ui->cbLineEndings->setCurrentIndex(le ? le - 1 : 0);

	int flags = QEditor::defaultFlags();
	ui->chkReplaceTabs->setChecked(flags & QEditor::ReplaceTabs);
	ui->chkAutoRemoveTrailingWhitespace->setChecked(flags & QEditor::RemoveTrailing);
	ui->chkPreserveTrailingIndent->setChecked(flags & QEditor::PreserveTrailingIndent);

	QTextCodec *c = QEditor::defaultCodec();
	ui->cbEncoding->setCurrentIndex(ui->cbEncoding->findText(c ? c->name() : QTextCodec::codecForLocale()->name()));

	m_direct = oldDir;
}

/*!
	\brief Restore default values for all subcontrols

	\note The widgets are changed but these changes are NOT applied.
*/
void QEditConfig::restore()
{
	// restore default configuration

	bool oldDir = m_direct;

	m_direct = false;

	QFont font("Monospace", 10);
	font.setStyleHint(QFont::Courier);

	ui->cbFont->setFont(font);
	ui->spnFontSize->setValue(10);

	ui->spnTabWidth->setValue(4);

	ui->chkShowTabsInText->setChecked(true);
	ui->chkShowLeadingWhitespace->setChecked(true);
	ui->chkShowTrailingWhitespace->setChecked(true);

	ui->chkDetectLE->setChecked(true);
	ui->cbLineEndings->setCurrentIndex(0);

	ui->chkReplaceTabs->setChecked(false);
	ui->chkAutoRemoveTrailingWhitespace->setChecked(false);
	ui->chkPreserveTrailingIndent->setChecked(true);

	ui->cbEncoding->setCurrentIndex(ui->cbEncoding->findText(QTextCodec::codecForLocale()->name()));

	m_direct = oldDir;
}

/*!
	\brief Fills a settings map from the state of the subcontrols
*/
QMap<QString, QVariant> QEditConfig::dumpKeys() const
{
	QMap<QString, QVariant> m;

	QFont font = ui->cbFont->currentFont();
	font.setPointSize(ui->spnFontSize->value());

	m.insert("font", font);
	m.insert("tab_width", ui->spnTabWidth->value());

	m.insert("show_leading_whitespace", ui->chkShowLeadingWhitespace->isChecked());
	m.insert("show_trailing_whitespace", ui->chkShowTrailingWhitespace->isChecked());
	m.insert("show_tabs_in_text", ui->chkShowTabsInText->isChecked());

	m.insert("replace_tabs", ui->chkReplaceTabs->isChecked());
	m.insert("remove_trailing", ui->chkAutoRemoveTrailingWhitespace->isChecked());
	m.insert("preserve_trailing_indent", ui->chkPreserveTrailingIndent->isChecked());

	m.insert("encoding", ui->cbEncoding->currentText());

	if ( ui->chkDetectLE->isChecked() )
		m.insert("line_endings", (int)QDocument::Conservative);
	else
		m.insert("line_endings", ui->cbLineEndings->currentIndex() + 1);

	return m;
}

/*!
	\brief Fills the widget subcontrols from a settings map

	\param keys a key/value map that can be obtained from QSettings
*/
void QEditConfig::loadKeys(const QMap<QString, QVariant>& keys)
{
	//qDebug("loading %i keys", keys.count());

	// load
	QMap<QString, QVariant>::const_iterator it = keys.constBegin();

	while ( it != keys.constEnd() )
	{
		if ( it.key() == "font" )
		{
			QFont f = qvariant_cast<QFont>(*it);

			ui->cbFont->setCurrentFont(f);
			ui->spnFontSize->setValue(f.pointSize());

			if ( m_direct )
				QDocument::setFont(f);

			ui->lblSampleText->setFont(f);

		} else if ( it.key() == "tab_width" ) {
		        ui->spnTabWidth->setValue(it->toInt());

			if ( m_direct )
				on_spnTabWidth_valueChanged(it->toInt());
		} else if ( it.key() == "replace_tabs" ) {
		        ui->chkReplaceTabs->setChecked(it->toBool());
			if ( m_direct )
				on_chkReplaceTabs_toggled(it->toBool());
		} else if ( it.key() == "remove_trailing" ) {
		        ui->chkAutoRemoveTrailingWhitespace->setChecked(it->toBool());
			if ( m_direct )
				on_chkAutoRemoveTrailingWhitespace_toggled(it->toBool());
		} else if ( it.key() == "preserve_trailing_indent" ) {
		        ui->chkPreserveTrailingIndent->setChecked(it->toBool());
			if ( m_direct )
				on_chkPreserveTrailingIndent_toggled(it->toBool());
		} else if ( it.key() == "show_tabs_in_text" ) {
		        ui->chkShowTabsInText->setChecked(it->toBool());
			if ( m_direct )
				on_chkShowTabsInText_toggled(it->toBool());
		} else if ( it.key() == "show_leading_whitespace" ) {
		        ui->chkShowLeadingWhitespace->setChecked(it->toBool());
			if ( m_direct )
				on_chkShowLeadingWhitespace_toggled(it->toBool());
		} else if ( it.key() == "show_trailing_whitespace" ) {
		        ui->chkShowTrailingWhitespace->setChecked(it->toBool());
			if ( m_direct )
				on_chkShowTrailingWhitespace_toggled(it->toBool());
		} else if ( it.key() == "encoding" ) {
		        ui->cbEncoding->setCurrentIndex(ui->cbEncoding->findText(it->toString()));
			if ( m_direct )
				on_cbEncoding_currentIndexChanged(it->toString());
		} else if ( it.key() == "line_endings" ) {
			int le = it->toInt();

			if ( le )
			{
			        ui->chkDetectLE->setChecked(false);
			        ui->cbLineEndings->setCurrentIndex(le - 1);
			} else {
			        ui->cbLineEndings->setCurrentIndex(0);
			        ui->chkDetectLE->setChecked(true);
			}
		} else {
			/*
			qWarning("QEditConfig::loadKeys() fed with unsupported settings key : \"%s\" ",
					qPrintable(it.key())
					);
			*/
		}

		++it;
	}
}

/*!
	\brief Slot used to apply font size settings
*/
void QEditConfig::on_spnFontSize_valueChanged(int size)
{
	QFont font = ui->cbFont->currentFont();
	font.setPointSize(size);

	ui->lblSampleText->setFont(font);

	if ( m_direct )
	{
		QDocument::setFont(font);
		emit keyChanged("font", font);
	}
}

/*!
	\brief Slot used to apply font family settings
*/
void QEditConfig::on_cbFont_currentFontChanged(QFont font)
{
	font.setPointSize(ui->spnFontSize->value());
	ui->lblSampleText->setFont(font);

	if ( m_direct )
	{
		QDocument::setFont(font);
		emit keyChanged("font", font);
	}
}

/*!
	\brief Slot used to apply tab width settings
*/
void QEditConfig::on_spnTabWidth_valueChanged(int n)
{
	if ( m_direct )
	{
		QDocument::setTabStop(n);
		emit keyChanged("tab_width", n);
	}
}

/*!
	\brief Slot used to apply tabs replacement settings
*/
void QEditConfig::on_chkReplaceTabs_toggled(bool y)
{
	if ( m_direct )
	{
		// FIXME
		foreach ( QEditor *e, QEditor::m_editors )
		{
			e->setFlag(QEditor::ReplaceTabs, y);
		}
		emit keyChanged("replace_tabs", y);
	}
}

/*!
	\brief Slot used to apply tabs display settings
*/
void QEditConfig::on_chkShowTabsInText_toggled(bool y)
{
	if ( m_direct )
	{
		if ( y )
			QDocument::setShowSpaces(QDocument::showSpaces() | QDocument::ShowTabs);
		else
			QDocument::setShowSpaces(QDocument::showSpaces() & ~QDocument::ShowTabs);

		emit keyChanged("show_tabs_in_text", y);
	}
}

/*!
	\brief Slot used to apply trailing whitespace display settings
*/
void QEditConfig::on_chkShowLeadingWhitespace_toggled(bool y)
{
	if ( m_direct )
	{
		if ( y )
			QDocument::setShowSpaces(QDocument::showSpaces() | QDocument::ShowLeading);
		else
			QDocument::setShowSpaces(QDocument::showSpaces() & ~QDocument::ShowLeading);

		emit keyChanged("show_leading_whitespace", y);
	}
}

/*!
	\brief Slot used to apply leading whitespace display settings
*/
void QEditConfig::on_chkShowTrailingWhitespace_toggled(bool y)
{
	if ( m_direct )
	{
		if ( y )
			QDocument::setShowSpaces(QDocument::showSpaces() | QDocument::ShowTrailing);
		else
			QDocument::setShowSpaces(QDocument::showSpaces() & ~QDocument::ShowTrailing);

		emit keyChanged("show_trailing_whitespace", y);
	}
}

/*!
	\brief Slot used to apply encodings settings
*/
void QEditConfig::on_cbEncoding_currentIndexChanged(const QString& name)
{
	if ( m_direct )
	{
		QEditor::setDefaultCodec(name.toLatin1(), QEditor::UpdateAll);
		emit keyChanged("encoding", name);
	}
}

/*!
	\brief Slot used to apply line endings settings
*/
void QEditConfig::on_cbLineEndings_currentIndexChanged(int idx)
{
	QDocument::LineEnding le = QDocument::LineEnding(ui->cbLineEndings->currentIndex() + 1);

	if ( m_direct )
	{
		QDocument::setDefaultLineEnding(le);
		emit keyChanged("line_endings", (int)le);
	}
}

/*!
	\brief Slot used to apply line endings auto detectionl settings
*/
void QEditConfig::on_chkDetectLE_toggled(bool y)
{
	QDocument::LineEnding le = QDocument::Conservative;

	if ( !y )
	{
		le = QDocument::LineEnding(ui->cbLineEndings->currentIndex() + 1);
	}

	if ( m_direct )
	{
		QDocument::setDefaultLineEnding(le);
		emit keyChanged("line_endings", (int)le);
	}
}

/*!
	\brief Slot used to apply trailing space removal settings
*/
void QEditConfig::on_chkAutoRemoveTrailingWhitespace_toggled(bool y)
{
	if ( m_direct )
	{
		// FIXME
		foreach ( QEditor *e, QEditor::m_editors )
		{
			e->setFlag(QEditor::RemoveTrailing, y);
		}
		emit keyChanged("remove_trailing", y);
	}
}

/*!
	\brief Slot used to indent preservation settings
*/
void QEditConfig::on_chkPreserveTrailingIndent_toggled(bool y)
{
	if ( m_direct )
	{
		// FIXME
		foreach ( QEditor *e, QEditor::m_editors )
		{
			e->setFlag(QEditor::PreserveTrailingIndent, y);
		}
		emit keyChanged("preserve_trailing_indent", y);
	}
}

/*! @} */

