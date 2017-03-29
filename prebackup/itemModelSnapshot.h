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
	void setSnapshot(std::shared_ptr<Snapshot>);
	std::shared_ptr<Snapshot> getSnapshot() const { return snapshot; }
	void sortRequested(int, Qt::SortOrder);

	QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

	QModelIndex index(int row, int column,  const QModelIndex &parent = QModelIndex()) const override;
	QModelIndex parent(const QModelIndex &index) const override;

	int rowCount(const QModelIndex &parent = QModelIndex()) const override;
	int columnCount(const QModelIndex &parent = QModelIndex()) const override;

	QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

private:
	std::shared_ptr<Snapshot> snapshot;
	std::pair<int, bool> currentSorting { -1, false };
	QVariant getContents(const QModelIndex &index, int role, bool header) const;  // used for both data and headers
	void sortSnapshot(int column, bool desc);
};


#endif // ITEMMODELSNAPSHOT_H
