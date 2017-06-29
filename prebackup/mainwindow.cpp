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


#include "mainwindow.h"
#include "itemModelSnapshot.h"
#include "snapshot.h"
#include "dialogRoots.h"
#include "dialogListChoose.h"
#include <QSettings>
#include <QDebug>
#include <vector>
#include <string>
#include <QMessageBox>
#include <QApplication>
#include <QVBoxLayout>
#include <QTreeView>
#include <QAction>
#include <QMenuBar>
#include <QHeaderView>
#include <QStatusBar>
#include <QFileInfo>
#include <QDir>
#include <QDateTime>
#include <QCloseEvent>
#include <QLabel>
#include <QGroupBox>
using namespace std;



// date format used as file name for saved snapshots
const QString MainWindow::fileDateFormat("yyyy-MM-dd-HH-mm-ss");

// date format used in GUI
const Qt::DateFormat MainWindow::userDateFormat = Qt::DefaultLocaleShortDate;


// helper class to temporarily change cursor to hourglass
class WaitCursor {
public:
	WaitCursor() { QApplication::setOverrideCursor(QCursor(Qt::WaitCursor)); }
	~WaitCursor() { QApplication::restoreOverrideCursor(); }
};




MainWindow::MainWindow(QWidget *parent) :
	QMainWindow(parent)
{
	setWindowTitle("prebackup");
	resize(640, 480);
	auto *centralWidget = new QWidget(this);
	auto *verticalLayout = new QVBoxLayout(centralWidget);
	verticalLayout->setContentsMargins(8, 8, 8, 8);
	auto *laySummary = new QHBoxLayout;
	verticalLayout->addLayout(laySummary);
	auto *gb = new QGroupBox("Current snapshot", this);
	laySummary->addWidget(gb);
	auto *laygb = new QVBoxLayout(gb);
	labelSnapDate = new QLabel(gb);
	labelSnapSize = new QLabel(gb);
	labelSnapDiff = new QLabel(gb);
	laygb->addWidget(labelSnapDate);
	laygb->addWidget(labelSnapSize);
	laygb->addWidget(labelSnapDiff);
	laygb->addStretch();  // align labels at top
	auto *gb2 = new QGroupBox("Comparing snapshot", this);
	laySummary->addWidget(gb2);
	auto *laygb2 = new QVBoxLayout(gb2);
	labelSnapCompareDate = new QLabel(gb2);
	labelSnapCompareSize = new QLabel(gb2);
	laygb2->addWidget(labelSnapCompareDate);
	laygb2->addWidget(labelSnapCompareSize);
	laygb2->addStretch();  // align labels at top
	treeView = new QTreeView(centralWidget);
	treeView->setAlternatingRowColors(true);
	treeView->setSortingEnabled(true);
	verticalLayout->addWidget(treeView, 1);
	setCentralWidget(centralWidget);
	statusBar();

	QMenu *menuSnapshot = menuBar()->addMenu("&Snapshot");
	actionRoots = menuSnapshot->addAction(QIcon(), "Select &main directories...", [this](){ DialogRoots(this).exec(); });
	actionRoots->setStatusTip("Select which directories to scan");
	actionScan = menuSnapshot->addAction(QIcon(), "Scan &new snapshot...", this, &MainWindow::scanNew);
	actionScan->setStatusTip("Scan a new snapshot");
	actionScanRoot = menuSnapshot->addAction(QIcon(), "Scan new snapshot as &root...", this, &MainWindow::scanNew);
	menuSnapshot->addSeparator();
	actionOpen = menuSnapshot->addAction(QIcon::fromTheme("document-open"), "&Open snapshot...",
		this, &MainWindow::snapshotOpen, QKeySequence::Open);
	actionOpen->setStatusTip("Open a previously saved snapshot");
	actionSave = menuSnapshot->addAction(QIcon::fromTheme("document-save"), "&Save snapshot...",
		this, &MainWindow::snapshotSave, QKeySequence::Save);
	actionSave->setStatusTip("Save the current snapshot");

	QMenu *menuCompare = menuBar()->addMenu("&Compare");
	actionCompareOpen = menuCompare->addAction(QIcon::fromTheme("document-open"), "&Open snapshot for comparison...",
		this, &MainWindow::compareOpen);
	actionCompareOpen->setStatusTip("Open an existing snapshot for comparison");
	actionCompareClose = menuCompare->addAction(QIcon::fromTheme("document-close"), "&Close comparison snapshot",
		this, &MainWindow::compareClose);
	actionCompareClose->setStatusTip("Close comparison snapshot");

	snapshotModel = new ItemModelSnapshot(this);
	treeView->setModel(snapshotModel);
	treeView->header()->setSectionsMovable(false);
	treeView->header()->setStretchLastSection(false);
	treeView->header()->setSectionResizeMode(0, QHeaderView::Stretch);
	currentSorting = { (int)ItemModelSnapshot::Column::TotSize, Qt::DescendingOrder };
	connect(treeView->header(), &QHeaderView::sortIndicatorChanged, this, &MainWindow::sortIndicatorChanged);
	treeView->header()->setSortIndicator(currentSorting.first, currentSorting.second);

	// create a QSettings object with ini format to get the directory in user's home where to save snapshot files
	QSettings sett(QSettings::IniFormat, QSettings::UserScope, QCoreApplication::organizationName(), QCoreApplication::applicationName());
	savePath = QFileInfo(sett.fileName()).absolutePath();
	QDir(savePath).mkpath(".");  // ensure path exists

	updateGui();
}


void MainWindow::updateGui() {
	actionSave->setEnabled(!snapshotModel->getSnapshot()->isSaved());

	labelSnapDate->setText("Date: " +
		(snapshotModel->getSnapshot()->isEmpty() ? "-" :
		QDateTime::fromMSecsSinceEpoch(snapshotModel->getSnapshot()->getTimestamp() * qint64(1000)).toString(userDateFormat)));
	labelSnapSize->setText("Size: " +
		(snapshotModel->getSnapshot()->isEmpty() ? "-" :
		QString::fromStdString(Snapshot::sizeToText(snapshotModel->getSnapshot()->getTotSize()))));
	labelSnapDiff->setText("Difference: " +
		(snapshotModel->getComparedSnapshot()->isEmpty() ? "-" :
		QString::fromStdString(Snapshot::relSizeToText(
			(int64_t)snapshotModel->getSnapshot()->getTotSize() - (int64_t)snapshotModel->getComparedSnapshot()->getTotSize()
		))));

	labelSnapCompareDate->setText("Date: " +
		(snapshotModel->getComparedSnapshot()->isEmpty() ? "-" :
		QDateTime::fromMSecsSinceEpoch(snapshotModel->getComparedSnapshot()->getTimestamp() * qint64(1000)).toString(userDateFormat)));
	labelSnapCompareSize->setText("Size: " +
		(snapshotModel->getComparedSnapshot()->isEmpty() ? "-" :
		QString::fromStdString(Snapshot::sizeToText(snapshotModel->getComparedSnapshot()->getTotSize()))));
}


bool MainWindow::checkDataSaved() {
	if (snapshotModel->getSnapshot()->isSaved()) return true;
	auto reply = QMessageBox::question(this, "Save snapshot", "Save current snapshot?",
		QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel, QMessageBox::Yes);
	if (reply == QMessageBox::No) return true;
	else if (reply == QMessageBox::Yes) return snapshotSave();
	else return false;
}


void MainWindow::closeEvent(QCloseEvent *event) {
	if (!checkDataSaved()) event->ignore();
}


void MainWindow::scanNew() {
	if (!checkDataSaved()) return;
	auto roots = QSettings().value("roots").toStringList();
	if (!roots.size()) {
		QMessageBox::critical(this, "No directories", "You must add one or more main directories first.");
		DialogRoots d(this);
		if (d.exec() != QDialog::Accepted) return;
		roots = QSettings().value("roots").toStringList();
		if (!roots.size()) return;
	}
	vector<string> vroots;
	for (auto const &r: roots) vroots.push_back(r.toStdString());
	WaitCursor _;
	shared_ptr<Snapshot> snap(new Snapshot(vroots));
	snapshotModel->setSnapshot(snap);
	sortIndicatorChanged(currentSorting.first, currentSorting.second);  // make new snapshot sort
	updateGui();
}


shared_ptr<Snapshot> MainWindow::loadSnapshot() {
	shared_ptr<Snapshot> result;
	// get list of saved snapshots
	QDir d = QDir(savePath);
	d.setFilter(QDir::Files);
	d.setNameFilters(QStringList() << "*.snapshot");
	d.setSorting(QDir::Name | QDir::Reversed);
	QStringList tmpList = d.entryList(), userList;
	std::map<QString, QString> nameMap;  // from user-friendly date to file name
	for (auto f: tmpList) {
		QString name = QFileInfo(f).baseName();
		QDateTime dt = QDateTime::fromString(name, fileDateFormat);
		if (dt.isValid()) {
			userList << dt.toString(userDateFormat);
			nameMap[dt.toString(userDateFormat)] = name;
		}
	}
	if (userList.empty()) {
		QMessageBox::critical(this, "No snapshots", "No saved snapshots found");
		return result;
	}
	DialogListChoose dlg(this, "Open snapshot", "Select snapshot to open:", userList);
	if (dlg.exec() != QDialog::Accepted) return result;
	QString fileName = dlg.result;

	fileName = QDir(savePath).absoluteFilePath(nameMap[fileName] + ".snapshot");
	try {
		WaitCursor _;
		result.reset(Snapshot::load(fileName.toStdString()));
	}
	catch (std::exception &) {
		QMessageBox::critical(this, "Error", "Unable to load file!");
	}
	return result;
}


void MainWindow::snapshotOpen() {
	if (!checkDataSaved()) return;
	auto s = loadSnapshot();
	if (!s) return;
	WaitCursor _;
	snapshotModel->setSnapshot(s);
	sortIndicatorChanged(currentSorting.first, currentSorting.second);  // make new snapshot sort
	updateGui();
}


bool MainWindow::snapshotSave() {
	auto snapTime = QDateTime::fromMSecsSinceEpoch(snapshotModel->getSnapshot()->getTimestamp() * qint64(1000));
	QString fileName = QDir(savePath).absoluteFilePath(snapTime.toString(fileDateFormat) + ".snapshot");
	try {
		WaitCursor _;
		snapshotModel->getSnapshot()->save(fileName.toStdString());
	}
	catch (std::exception &) {
		QMessageBox::critical(this, "Error", "Unable to save file!");
		return false;
	}
	statusBar()->showMessage("Snapshot saved to " + fileName);
	updateGui();
	return true;
}


void MainWindow::compareOpen() {
	auto s = loadSnapshot();
	if (!s) return;
	WaitCursor _;
	snapshotModel->setComparedSnapshot(s);
	sortIndicatorChanged(currentSorting.first, currentSorting.second);  // make snapshot sort
	updateGui();
}


void MainWindow::compareClose() {
	WaitCursor _;
	snapshotModel->removeComparedSnapshot();
	sortIndicatorChanged(currentSorting.first, currentSorting.second);  // make snapshot sort
	updateGui();
}


void MainWindow::sortIndicatorChanged(int column, Qt::SortOrder order) {
	if (ItemModelSnapshot::isColumnSortable(column)) {
		currentSorting = { column, order };
		WaitCursor _;
		snapshotModel->sortRequested(column, order);
	}
	else treeView->header()->setSortIndicator(currentSorting.first, currentSorting.second);
}
