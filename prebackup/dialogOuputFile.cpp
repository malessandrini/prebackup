/*
Prebackup
Copyright (C) 2017  Michele Alessandrini

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License version 3
as published by the Free Software Foundation.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/


#include "dialogOuputFile.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPlainTextEdit>
#include <QDialogButtonBox>
#include <QLabel>
#include <QIcon>
#include <QSettings>
#include <QFileDialog>
#include <QFile>
#include <QTextStream>
#include <QMessageBox>
#include <QFont>


DialogOutputFile::DialogOutputFile(QWidget *parent, const QString &title, const QString &label,
		const QStringList &items, const QString &warning, const QString &setting):
	QDialog(parent), settingName(setting)
{
	setWindowTitle(title);
	resize(600, 400);
	auto *lay1 = new QVBoxLayout(this);
	lay1->addWidget(new QLabel(label, this));
	textBox = new QPlainTextEdit(this);
	textBox->setReadOnly(true);
	for (auto s: items) textBox->appendPlainText(s);
	lay1->addWidget(textBox, 1);
	if (warning.size()) {
		auto *l = new QHBoxLayout;
		lay1->addLayout(l);
		QLabel *p = new QLabel(this);
		p->setPixmap(QIcon::fromTheme("dialog-warning", QIcon(":/res/Dialog-warning.png")).pixmap(48, 48));
		l->addWidget(p);
		QLabel *w = new QLabel(tr("Warning: ") + warning, this);
		w->setWordWrap(true);
		QFont font = w->font();
		font.setBold(true);
		w->setFont(font);
		l->addWidget(w, 1);
	}
	bbox = new QDialogButtonBox(QDialogButtonBox::Save | QDialogButtonBox::Close,
		Qt::Horizontal, this);
	lay1->addWidget(bbox);
	connect(bbox, &QDialogButtonBox::accepted, this, &DialogOutputFile::saveFile);
	connect(bbox, &QDialogButtonBox::rejected, this, &QDialog::reject);
}


void DialogOutputFile::saveFile() {
	QString lastFile = QSettings().value(settingName).toString();
	QString saveFile = QFileDialog::getSaveFileName(this, QString(), lastFile);
	if (!saveFile.size()) return;
	QSettings().setValue(settingName, saveFile);
	QFile f(saveFile);
	if (!f.open(QIODevice::WriteOnly | QIODevice::Text)) {
		QMessageBox::critical(this, tr("Write error"), tr("Error writing file"));
		return;
	}
	QTextStream ts(&f);
	ts << textBox->toPlainText() << "\n";
}
