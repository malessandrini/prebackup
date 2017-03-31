#include "mainwindow.h"
#include "itemModelSnapshot.h"
#include "snapshot.h"
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
using namespace std;



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
	treeView = new QTreeView(centralWidget);
	treeView->setAlternatingRowColors(true);
	treeView->setSortingEnabled(true);
	verticalLayout->addWidget(treeView);
	setCentralWidget(centralWidget);
	statusBar();

	QMenu *menuSnapshot = menuBar()->addMenu("&Snapshot");
	actionScan = menuSnapshot->addAction(QIcon(), "New snapshot...", this, &MainWindow::scanNew);
	actionScan->setStatusTip("Scan a new snapshot");
	actionScanRoot = menuSnapshot->addAction(QIcon(), "New snapshot as root...", this, &MainWindow::scanNew);  // TODO
	menuSnapshot->addSeparator();
	actionOpen = menuSnapshot->addAction(QIcon::fromTheme("document-open"), "&Open snapshot...",
		this, &MainWindow::snapshotOpen, QKeySequence::Open);
	actionOpen->setStatusTip("Open a previously saved snapshot");
	actionSave = menuSnapshot->addAction(QIcon::fromTheme("document-save"), "&Save snapshot...",
		this, &MainWindow::snapshotSave, QKeySequence::Save);
	actionSave->setStatusTip("Save the current snapshot");
	// TODO: enable save only if needed, and check at exit

	snapshotModel = new ItemModelSnapshot(this);
	treeView->setModel(snapshotModel);
	treeView->header()->setSectionsMovable(false);
	currentSorting = { 2, Qt::DescendingOrder };
	connect(treeView->header(), &QHeaderView::sortIndicatorChanged, this, &MainWindow::sortIndicatorChanged);
	treeView->header()->setSortIndicator(currentSorting.first, currentSorting.second);
}


void MainWindow::scanNew() {
	auto roots = QSettings().value("roots").toStringList();
	if (!roots.size()) {
		QMessageBox::critical(this, "No directories", "You must add one or more root directories first.");
		return;
	}
	vector<string> vroots;
	for (auto const &r: roots) vroots.push_back(r.toStdString());
	WaitCursor _;
	shared_ptr<Snapshot> snap(new Snapshot(vroots));
	snap->scan();
	snapshotModel->setSnapshot(snap);
	sortIndicatorChanged(currentSorting.first, currentSorting.second);  // make new snapshot sort
}


void MainWindow::snapshotOpen() {
	try {
		WaitCursor _;
		snapshotModel->setSnapshot(shared_ptr<Snapshot>(Snapshot::load("aaa.snapshot")));
		sortIndicatorChanged(currentSorting.first, currentSorting.second);  // make new snapshot sort
	}
	catch (std::exception &) {
		QMessageBox::critical(this, "Error", "Unable to load file!");
	}
}


void MainWindow::snapshotSave() {
	try {
		WaitCursor _;
		snapshotModel->getSnapshot()->save("aaa.snapshot");
		statusBar()->showMessage("Snapshot saved to xyz");
	}
	catch (std::exception &) {
		QMessageBox::critical(this, "Error", "Unable to save file!");
	}
}


void MainWindow::sortIndicatorChanged(int column, Qt::SortOrder order)
{
	switch (column) {
	case 0:
	case 2:
	case 3:
		currentSorting = { column, order };
		WaitCursor _;
		snapshotModel->sortRequested(column, order);
		return;
	}
	// invalid column
	treeView->header()->setSortIndicator(currentSorting.first, currentSorting.second);
}
