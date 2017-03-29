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
	actionScan = menuSnapshot->addAction(QIcon(), "Scan...", this, &MainWindow::scanNew);
	actionScanRoot = menuSnapshot->addAction(QIcon(), "Scan as root...", this, &MainWindow::scanNew);  // TODO
	menuSnapshot->addSeparator();
	actionOpen = menuSnapshot->addAction(QIcon::fromTheme("document-open"), "&Open previous...",
		this, &MainWindow::snapshotOpen, QKeySequence::Open);
	actionSave = menuSnapshot->addAction(QIcon::fromTheme("document-save"), "&Save snapshot...",
		this, &MainWindow::snapshotSave, QKeySequence::Save);

	treeView->setModel(new ItemModelSnapshot(this));
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
	dynamic_cast<ItemModelSnapshot*>(treeView->model())->setSnapshot(snap);
	sortIndicatorChanged(currentSorting.first, currentSorting.second);  // make new snapshot sort
}


void MainWindow::snapshotOpen() {

}


void MainWindow::snapshotSave() {
	try {
		WaitCursor _;
		dynamic_cast<ItemModelSnapshot*>(treeView->model())->getSnapshot()->save("aaa.snapshot");
	}
	catch (std::exception &) {
		QMessageBox::critical(this, "Error", "Unable to save file!");
	}
	// TODO: write to status bar?
}


void MainWindow::sortIndicatorChanged(int column, Qt::SortOrder order)
{
	switch (column) {
	case 0:
	case 2:
	case 3:
		currentSorting = { column, order };
		WaitCursor _;
		dynamic_cast<ItemModelSnapshot*>(treeView->model())->sortRequested(column, order);
		return;
	}
	// invalid column
	treeView->header()->setSortIndicator(currentSorting.first, currentSorting.second);
}
