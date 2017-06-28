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


#ifndef ITEMMODELSNAPSHOT_H
#define ITEMMODELSNAPSHOT_H

#include <QAbstractItemModel>
#include <memory>
#include <map>


class Snapshot;
class Directory;


class ItemModelSnapshot : public QAbstractItemModel {
	Q_OBJECT
public:
	ItemModelSnapshot(QObject *parent);
	ItemModelSnapshot(ItemModelSnapshot const&) = delete;
	ItemModelSnapshot& operator=(ItemModelSnapshot const&) = delete;
	void setSnapshot(std::shared_ptr<Snapshot>);
	void setComparedSnapshot(std::shared_ptr<Snapshot>);
	void removeComparedSnapshot();
	const Snapshot * getSnapshot() const { return snapshot.get(); }
	const Snapshot * getComparedSnapshot() const { return comparedSnapshot.get(); }
	void sortRequested(int, Qt::SortOrder);

	QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

	QModelIndex index(int row, int column,  const QModelIndex &parent = QModelIndex()) const override;
	QModelIndex parent(const QModelIndex &index) const override;

	int rowCount(const QModelIndex &parent = QModelIndex()) const override;
	int columnCount(const QModelIndex &parent = QModelIndex()) const override;

	QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

	enum class Column { Name, State, FileSize, TotSize, DiffSize, _ColumnCount_ };
	static bool isColumnSortable(int);

private:
	std::shared_ptr<Snapshot> snapshot, comparedSnapshot;
	std::pair<int, bool> currentSorting { -1, false };
	QVariant getContents(const QModelIndex &index, int role, bool header) const;  // used for both data and headers
	void sortSnapshot(int column, bool desc);
	void recomputeDifference();
};


#endif // ITEMMODELSNAPSHOT_H
