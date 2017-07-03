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
#include <QPlainTextEdit>
#include <QDialogButtonBox>
#include <QLabel>


DialogOutputFile::DialogOutputFile(QWidget *parent, const QString &title, const QString &label,
		const QStringList &items, const QString &warning, const QString &setting):
	QDialog(parent)
{
	setWindowTitle(title);
	resize(600, 400);
	auto *lay1 = new QVBoxLayout(this);
	lay1->addWidget(new QLabel(label, this));
	textBox = new QPlainTextEdit(this);
	textBox->setReadOnly(true);
	for (auto s: items) textBox->appendPlainText(s);
	lay1->addWidget(textBox, 1);
	bbox = new QDialogButtonBox(QDialogButtonBox::Save | QDialogButtonBox::Close,
		Qt::Horizontal, this);
	lay1->addWidget(bbox);
	connect(bbox, &QDialogButtonBox::accepted, this, &DialogOutputFile::saveFile);
	connect(bbox, &QDialogButtonBox::rejected, this, &QDialog::reject);
}


void DialogOutputFile::saveFile() {
}
