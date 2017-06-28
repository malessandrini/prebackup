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


#include "itemModelSnapshot.h"
#include "snapshot.h"
#include "directory.h"
#include <QStringList>
#include <QIcon>
#include <QBrush>
#include <algorithm>
#include <QDebug>
using namespace std;


ItemModelSnapshot::ItemModelSnapshot(QObject *parent)
	: QAbstractItemModel(parent), snapshot(new Snapshot({})), comparedSnapshot(new Snapshot({}))
{
}


void ItemModelSnapshot::setSnapshot(shared_ptr<Snapshot> sn) {
	beginResetModel();
	snapshot = sn;
	currentSorting = { -1, false };
	recomputeDifference();
	endResetModel();
}


void ItemModelSnapshot::setComparedSnapshot(std::shared_ptr<Snapshot> sn) {
	beginResetModel();
	comparedSnapshot = sn;
	recomputeDifference();
	endResetModel();
}


void ItemModelSnapshot::removeComparedSnapshot() {
	beginResetModel();
	comparedSnapshot.reset(new Snapshot({}));
	recomputeDifference();
	endResetModel();
}


QVariant ItemModelSnapshot::headerData(int section, Qt::Orientation orientation, int role) const
{
	if (orientation == Qt::Horizontal) return getContents(createIndex(0, section, nullptr), role, true);
	return QVariant();
}


QModelIndex ItemModelSnapshot::index(int row, int column, const QModelIndex &parent) const
{
	vector<Directory*>::const_iterator b, e;
	if (!parent.isValid()) {
		// top-level item
		b = snapshot->cbegin();
		e = snapshot->cend();
	}
	else {
		const Directory *parentDir = static_cast<const Directory*>(parent.internalPointer());
		if (!parentDir) return QModelIndex();
		b = parentDir->cbegin();
		e = parentDir->cend();
	}
	if (row >= (e - b) || column >= (int)Column::_ColumnCount_) return QModelIndex();
	return createIndex(row, column, const_cast<Directory*>(*(b + row)));
}


QModelIndex ItemModelSnapshot::parent(const QModelIndex &index) const
{
	if (!index.isValid()) return QModelIndex();
	const Directory *dir = static_cast<const Directory*>(index.internalPointer()), *parentDir;
	if (!dir || !(parentDir = dir->getParent())) return QModelIndex();
	// we have to find the parent's row in parent's parent's vector!
	vector<Directory*>::const_iterator b, e;
	if (parentDir->getParent()) {
		 b = parentDir->getParent()->cbegin();
		 e = parentDir->getParent()->cend();
	}
	else {
		b = snapshot->cbegin();
		e = snapshot->cend();
	}
	auto i = std::find(b, e, parentDir);
	return createIndex((i == e ? /*should not happen*/ 0 : (i - b)), 0, const_cast<Directory*>(parentDir));
}


int ItemModelSnapshot::rowCount(const QModelIndex &parent) const
{
	if (parent.column() > 0) return 0;
	if (!parent.isValid()) return snapshot->cend() - snapshot->cbegin();
	const Directory *dir = static_cast<const Directory*>(parent.internalPointer());
	return dir->cend() - dir->cbegin();
}


int ItemModelSnapshot::columnCount(const QModelIndex &) const
{
	return (int)Column::_ColumnCount_;
}


bool ItemModelSnapshot::isColumnSortable(int column) {
	switch(column) {
	case (int)Column::Name:
	case (int)Column::FileSize:
	case (int)Column::TotSize:
	case (int)Column::DiffSize:
		return true;
	}
	return false;
}


QVariant ItemModelSnapshot::data(const QModelIndex &index, int role) const
{
	return getContents(index, role, false);
}


QVariant ItemModelSnapshot::getContents(const QModelIndex &index, int role, bool header) const
{
	if (!index.isValid() || (header && role != Qt::DisplayRole)) return QVariant();
	const Directory *dir = static_cast<const Directory*>(index.internalPointer());
	if (!header && !dir) return QVariant();

	switch (index.column()) {
	case (int)Column::Name:
		if (header) return QString("directory");
		if (role == Qt::DisplayRole) return QString::fromStdString(dir->getName());
		if (role == Qt::DecorationRole) return QIcon::fromTheme("folder", QIcon(":/res/Folder.png"));
		if (role == Qt::ForegroundRole) {
			if (dir->isExcluded()) return QBrush(Qt::gray);
		}
		if (role == Qt::BackgroundRole) {
			if (dir->isGhost()) return QBrush(Qt::gray);
		}
		break;
	case (int)Column::State:
		if (header) return QString("state");
		if (role == Qt::DecorationRole) {
			if (dir->hasErrors()) return QIcon::fromTheme("dialog-warning", QIcon(":/res/Dialog-warning.png"));
			if (dir->isExcluded()) return QIcon::fromTheme("edit-delete", QIcon(":/res/Edit-delete.png"));
		}
		if (role == Qt::ToolTipRole) {
			if (dir->hasErrors()) return QString("Read errors");
			if (dir->isExcluded()) return QString("Excluded");
		}
		break;
	case (int)Column::FileSize:
		if (header) return QString("file size");
		if (role == Qt::DisplayRole) return QString::fromStdString(Snapshot::sizeToText(dir->getFileSize()));
		if (role == Qt::ForegroundRole) {
			if (dir->isExcluded() || dir->isGhost()) return QBrush(Qt::gray);
		}
		break;
	case (int)Column::TotSize:
		if (header) return QString("total size");
		if (role == Qt::DisplayRole) return QString::fromStdString(Snapshot::sizeToText(dir->getTotSize()));
		if (role == Qt::ForegroundRole) {
			if (dir->isExcluded() || dir->isGhost()) return QBrush(Qt::gray);
		}
		break;
	case (int)Column::DiffSize:
		if (header) return QString("diff size");
		if (role == Qt::DisplayRole) return QString::fromStdString(Snapshot::relSizeToText(dir->getDiffSize()));
		if (role == Qt::ForegroundRole) {
			if (comparedSnapshot->isEmpty()) return QBrush(Qt::gray);
		}
		break;
	}

	return QVariant();
}


void ItemModelSnapshot::sortRequested(int column, Qt::SortOrder order)
{
	if (!isColumnSortable(column)) return;
	//qDebug() << "Requested sorting" << column << order;
	pair<int, bool> newSorting = { column, (bool)order };
	if (newSorting == currentSorting) {
		//qDebug() << "Sorting skipped";
		return;
	}
	currentSorting = newSorting;
	sortSnapshot(newSorting.first, newSorting.second);
}


void ItemModelSnapshot::sortSnapshot(int column, bool desc)
{
	beginResetModel();
	switch (column) {
	case (int)Column::Name:
		snapshot->sortSubDirs([desc](const Directory *a, const Directory *b){
			return desc ? a->getName() > b->getName() :  a->getName() < b->getName(); });
		break;
	case (int)Column::FileSize:
		snapshot->sortSubDirs([desc](const Directory *a, const Directory *b){
			return desc ? a->getFileSize() > b->getFileSize() :  a->getFileSize() < b->getFileSize(); });
		break;
	case (int)Column::TotSize:
		snapshot->sortSubDirs([desc](const Directory *a, const Directory *b){
			return desc ? a->getTotSize() > b->getTotSize() :  a->getTotSize() < b->getTotSize(); });
		break;
	case (int)Column::DiffSize:
		snapshot->sortSubDirs([desc](const Directory *a, const Directory *b){
			return desc ? a->getDiffSize() > b->getDiffSize() :  a->getDiffSize() < b->getDiffSize(); });
		break;
	}
	endResetModel();
}


void ItemModelSnapshot::recomputeDifference() {
	if (snapshot->isEmpty()) return;
	currentSorting = { -1, false };  // sorting will be no more defined
	snapshot->recomputeDifference(comparedSnapshot.get());
}


