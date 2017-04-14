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
	ItemModelSnapshot(QObject *parent = 0);
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

	enum class Column { Name, State, TotSize, FileSize, DiffSize, _ColumnCount_ };
	static bool isColumnSortable(int);

private:
	std::shared_ptr<Snapshot> snapshot, comparedSnapshot;
	std::pair<int, bool> currentSorting { -1, false };
	QVariant getContents(const QModelIndex &index, int role, bool header) const;  // used for both data and headers
	void sortSnapshot(int column, bool desc);
	void recomputeDifference();
};


#endif // ITEMMODELSNAPSHOT_H
