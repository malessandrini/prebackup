#ifndef ITEMMODELSNAPSHOT_H
#define ITEMMODELSNAPSHOT_H

#include <QAbstractItemModel>
#include <memory>
#include <map>


class Snapshot;
class Directory;


class ItemModelSnapshot : public QAbstractItemModel
{
	Q_OBJECT

public:
	ItemModelSnapshot(QObject *parent = 0);
	void setSnapshot(std::shared_ptr<Snapshot>);

	QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

	QModelIndex index(int row, int column,  const QModelIndex &parent = QModelIndex()) const override;
	QModelIndex parent(const QModelIndex &index) const override;

	int rowCount(const QModelIndex &parent = QModelIndex()) const override;
	int columnCount(const QModelIndex &parent = QModelIndex()) const override;

	QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

private:
	std::shared_ptr<Snapshot> snapshot;
};


#endif // ITEMMODELSNAPSHOT_H
