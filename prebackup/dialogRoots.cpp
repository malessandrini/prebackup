#include "dialogRoots.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QListWidget>
#include <QToolBar>
#include <QAction>
#include <QDialogButtonBox>
#include <QSettings>
#include <QFileDialog>


DialogRoots::DialogRoots(QWidget *parent):
	QDialog(parent)
{
	setWindowTitle("Root directories");
	auto *lay1 = new QVBoxLayout(this);
//	setLayout(lay1);
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
	actionAdd = toolBar->addAction(QIcon(":/res/List-add.png"), "Add", this, &DialogRoots::itemAdd);
	actionDelete = toolBar->addAction(QIcon(":/res/List-remove.png"), "Delete", this, &DialogRoots::itemDelete);
	actionUp = toolBar->addAction(QIcon(":/res/Go-up.png"), "Move up", this, &DialogRoots::itemUp);
	actionDown = toolBar->addAction(QIcon(":/res/Go-down.png"), "Move down", this, &DialogRoots::itemDown);
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
