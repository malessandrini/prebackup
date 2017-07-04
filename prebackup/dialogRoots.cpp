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


#include "dialogRoots.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QListWidget>
#include <QToolBar>
#include <QAction>
#include <QDialogButtonBox>
#include <QSettings>
#include <QFileDialog>


DialogRoots::DialogRoots(QWidget *parent, QString const &title):
	QDialog(parent)
{
	setWindowTitle(title);
	auto *lay1 = new QVBoxLayout(this);
	auto *lay2 = new QHBoxLayout;
	lay1->addLayout(lay2, 1);
	listRoots = new QListWidget(this);
	lay2->addWidget(listRoots, 1);
	listRoots->setSelectionMode(QAbstractItemView::SingleSelection);
	auto *toolBar = new QToolBar(this);
	toolBar->setOrientation(Qt::Vertical);
	toolBar->setMovable(false);
	toolBar->setFloatable(false);
	lay2->addWidget(toolBar);
	actionAdd = toolBar->addAction(QIcon(":/res/List-add.png"), tr("Add"), this, &DialogRoots::itemAdd);
	actionDelete = toolBar->addAction(QIcon(":/res/List-remove.png"), tr("Delete"), this, &DialogRoots::itemDelete);
	actionUp = toolBar->addAction(QIcon(":/res/Go-up.png"), tr("Move up"), this, &DialogRoots::itemUp);
	actionDown = toolBar->addAction(QIcon(":/res/Go-down.png"), tr("Move down"), this, &DialogRoots::itemDown);
	auto *bbox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel,
		Qt::Horizontal, this);
	lay1->addWidget(bbox);
	connect(bbox, &QDialogButtonBox::accepted, this, &DialogRoots::accept);
	connect(bbox, &QDialogButtonBox::rejected, this, &QDialog::reject);
	listRoots->addItems(QSettings().value("roots").toStringList());
	connect(listRoots, &QListWidget::itemSelectionChanged, this, &DialogRoots::selectionChanged);
	selectionChanged();
}


void DialogRoots::itemAdd() {
	QString d = QFileDialog::getExistingDirectory(this);
	if (!d.size()) return;
	listRoots->addItem(d);
}


void DialogRoots::itemDelete() {
	auto sel = listRoots->selectedItems();
	if (sel.size() == 1) delete sel[0];
}


void DialogRoots::itemUp() {
	auto sel = listRoots->selectedItems();
	if (sel.size() != 1) return;
	int row = listRoots->row(sel[0]);
	if (row < 1) return;
	listRoots->takeItem(row);
	listRoots->insertItem(row - 1, sel[0]);
	listRoots->setCurrentItem(sel[0]);
}


void DialogRoots::itemDown() {
	auto sel = listRoots->selectedItems();
	if (sel.size() != 1) return;
	int row = listRoots->row(sel[0]);
	if (row >= (listRoots->count() - 1)) return;
	listRoots->takeItem(row);
	listRoots->insertItem(row + 1, sel[0]);
	listRoots->setCurrentItem(sel[0]);
}


void DialogRoots::selectionChanged() {
	bool ok = listRoots->selectedItems().size() == 1;
	actionDelete->setEnabled(ok);
	actionUp->setEnabled(ok);
	actionDown->setEnabled(ok);
}


void DialogRoots::accept() {
	QStringList s;
	for (int i =0; i < listRoots->count(); ++i)
		s << listRoots->item(i)->text();
	QSettings().setValue("roots", s);
	QDialog::accept();
}
