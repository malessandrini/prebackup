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


#include "dialogListChoose.h"
#include <QVBoxLayout>
#include <QListWidget>
#include <QDialogButtonBox>
#include <QLabel>
#include <QPushButton>


DialogListChoose::DialogListChoose(QWidget *parent, const QString &title, const QString &label, const QStringList &items):
	QDialog(parent)
{
	setWindowTitle(title);
	auto *lay1 = new QVBoxLayout(this);
	lay1->addWidget(new QLabel(label, this));
	list = new QListWidget(this);
	lay1->addWidget(list, 1);
	list->setSelectionMode(QAbstractItemView::SingleSelection);
	bbox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel,
		Qt::Horizontal, this);
	lay1->addWidget(bbox);
	connect(bbox, &QDialogButtonBox::accepted, this, &QDialog::accept);
	connect(bbox, &QDialogButtonBox::rejected, this, &QDialog::reject);
	list->addItems(items);
	connect(list, &QListWidget::itemSelectionChanged, this, &DialogListChoose::selectionChanged);
	connect(list, &QListWidget::itemDoubleClicked, this, &DialogListChoose::doubleClicked);
	selectionChanged();
}


void DialogListChoose::selectionChanged() {
	bool ok = list->selectedItems().size() == 1;
	bbox->button(QDialogButtonBox::Ok)->setEnabled(ok);
	result = ok ? list->selectedItems()[0]->text() : QString();
}


void DialogListChoose::doubleClicked(QListWidgetItem *item) {
	result = item->text();
	accept();
}
