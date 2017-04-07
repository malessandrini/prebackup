#include "mainwindow.h"
#include "itemModelSnapshot.h"
#include "snapshot.h"
#include "dialogRoots.h"
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
#include <QInputDialog>
#include <QCloseEvent>
#include <QLabel>
#include <QGroupBox>
using namespace std;



const QString MainWindow::fileDateFormat("yyyy-MM-dd-HH-mm-ss");



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
	new QVBoxLayout(gb);
	labelSnapDate = new QLabel(gb);
	labelSnapSize = new QLabel(gb);
	gb->layout()->addWidget(labelSnapDate);
	gb->layout()->addWidget(labelSnapSize);
	auto *gb2 = new QGroupBox("Comparing snapshot", this);
	laySummary->addWidget(gb2);
	new QVBoxLayout(gb2);
	labelSnapCompareDate = new QLabel(gb2);
	labelSnapCompareSize = new QLabel(gb2);
	gb2->layout()->addWidget(labelSnapCompareDate);
	gb2->layout()->addWidget(labelSnapCompareSize);
	treeView = new QTreeView(centralWidget);
	treeView->setAlternatingRowColors(true);
	treeView->setSortingEnabled(true);
	verticalLayout->addWidget(treeView);
	setCentralWidget(centralWidget);
	statusBar();

	QMenu *menuSnapshot = menuBar()->addMenu("&Snapshot");
	actionRoots = menuSnapshot->addAction(QIcon(), "Select &main directories...", [this](){ DialogRoots(this).exec(); });
	actionRoots->setStatusTip("Select which directories to scan");
	actionScan = menuSnapshot->addAction(QIcon(), "Scan &new snapshot...", this, &MainWindow::scanNew);
	actionScan->setStatusTip("Scan a new snapshot");
	actionScanRoot = menuSnapshot->addAction(QIcon(), "Scan new snapshot as &root...", this, &MainWindow::scanNew);  // TODO
	menuSnapshot->addSeparator();
	actionOpen = menuSnapshot->addAction(QIcon::fromTheme("document-open"), "&Open snapshot...",
		this, &MainWindow::snapshotOpen, QKeySequence::Open);
	actionOpen->setStatusTip("Open a previously saved snapshot");
	actionSave = menuSnapshot->addAction(QIcon::fromTheme("document-save"), "&Save snapshot...",
		this, &MainWindow::snapshotSave, QKeySequence::Save);
	actionSave->setStatusTip("Save the current snapshot");

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
		QDateTime::fromMSecsSinceEpoch(snapshotModel->getSnapshot()->getTimestamp() * qint64(1000)).toString(fileDateFormat)));
	labelSnapSize->setText("Size: " +
		(snapshotModel->getSnapshot()->isEmpty() ? "-" :
		QString::fromStdString(Snapshot::sizeToText(snapshotModel->getSnapshot()->getTotSize()))));
}


void MainWindow::closeEvent(QCloseEvent *event) {
	if (snapshotModel->getSnapshot()->isSaved()) return;
	auto reply = QMessageBox::question(this, "Save snapshot", "Save current snapshot?",
		QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel, QMessageBox::Yes);
	if (reply == QMessageBox::No) return;
	else if (reply == QMessageBox::Yes) {
		if (!snapshotSave()) event->ignore();
	}
	else event->ignore();
}


void MainWindow::scanNew() {
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
	snap->scan();
	snapshotModel->setSnapshot(snap);
	sortIndicatorChanged(currentSorting.first, currentSorting.second);  // make new snapshot sort
	updateGui();
}


void MainWindow::snapshotOpen() {
	// get list of saved snapshots
	QDir d = QDir(savePath);
	d.setFilter(QDir::Files);
	d.setNameFilters(QStringList() << "*.snapshot");
	d.setSorting(QDir::Name | QDir::Reversed);
	QStringList tmpList = d.entryList(), snapList;
	for (auto f: tmpList) {
		QString name = QFileInfo(f).baseName();
		if (QDateTime::fromString(name, fileDateFormat).isValid())
			snapList << name;
	}
	if (snapList.empty()) {
		QMessageBox::critical(this, "No snapshots", "No saved snapshots found");
		return;
	}
	bool ok;
	QString fileName = QInputDialog::getItem(this, "Open snapshot", "Select snapshot to open:", snapList, 0, false, &ok);
	if (!ok || fileName.isEmpty()) return;
	fileName = QDir(savePath).absoluteFilePath(fileName + ".snapshot");
	try {
		WaitCursor _;
		snapshotModel->setSnapshot(shared_ptr<Snapshot>(Snapshot::load(fileName.toStdString())));
		sortIndicatorChanged(currentSorting.first, currentSorting.second);  // make new snapshot sort
	}
	catch (std::exception &) {
		QMessageBox::critical(this, "Error", "Unable to load file!");
	}
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


void MainWindow::sortIndicatorChanged(int column, Qt::SortOrder order) {
	if (ItemModelSnapshot::isColumnSortable(column)) {
		currentSorting = { column, order };
		WaitCursor _;
		snapshotModel->sortRequested(column, order);
	}
	else treeView->header()->setSortIndicator(currentSorting.first, currentSorting.second);
}
