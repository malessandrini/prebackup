#ifndef MAINWINDOW_H
#define MAINWINDOW_H


#include <QMainWindow>
#include <memory>
#include <utility>


class ItemModelSnapshot;
class QTreeView;


class MainWindow : public QMainWindow {
	Q_OBJECT
public:
	MainWindow(QWidget *parent = 0);

private:
	std::pair<int, Qt::SortOrder> currentSorting;
	QAction *actionScan, *actionScanRoot, *actionOpen, *actionSave;
	QTreeView *treeView;
	ItemModelSnapshot *snapshotModel;
	QString savePath;
	static const QString fileDateFormat;
	void updateGui();
	void closeEvent(QCloseEvent*) override;

private slots:
	void sortIndicatorChanged(int, Qt::SortOrder);
	void scanNew();
	void snapshotOpen();
	bool snapshotSave();
};


#endif // MAINWINDOW_H
