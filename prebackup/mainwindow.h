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
	MainWindow(QWidget *parent = nullptr);

private:
	std::pair<int, Qt::SortOrder> currentSorting;
	QAction *actionRoots, *actionScan, *actionScanRoot, *actionOpen, *actionSave, *actionCompareOpen, *actionCompareClose;
	QTreeView *treeView;
	ItemModelSnapshot *snapshotModel;
	QLabel *labelSnapDate, *labelSnapSize, *labelSnapDiff, *labelSnapCompareDate, *labelSnapCompareSize;
	QString savePath;
	static const QString fileDateFormat;
	static const Qt::DateFormat userDateFormat;
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
