#ifndef MAINWINDOW_H
#define MAINWINDOW_H


#include <QMainWindow>
#include <memory>
#include <utility>


class Snapshot;
class ItemModelSnapshot;
class QTreeView;
class QLabel;


class MainWindow : public QMainWindow {
	Q_OBJECT
public:
	MainWindow(QWidget *parent = 0);

private:
	std::pair<int, Qt::SortOrder> currentSorting;
	QAction *actionRoots, *actionScan, *actionScanRoot, *actionOpen, *actionSave, *actionCompareOpen, *actionCompareClose;
	QTreeView *treeView;
	ItemModelSnapshot *snapshotModel;
	QLabel *labelSnapDate, *labelSnapSize, *labelSnapDiff, *labelSnapCompareDate, *labelSnapCompareSize;
	QString savePath;
	static const QString fileDateFormat;
	void updateGui();
	bool checkDataSaved();
	void closeEvent(QCloseEvent*) override;
	std::shared_ptr<Snapshot> loadSnapshot();

private slots:
	void sortIndicatorChanged(int, Qt::SortOrder);
	void scanNew();
	void snapshotOpen();
	bool snapshotSave();
	void compareOpen();
	void compareClose();
};


#endif // MAINWINDOW_H
