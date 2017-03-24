#include "itemModelSnapshot.h"
#include "snapshot.h"
#include "directory.h"
using namespace std;


ItemModelSnapshot::ItemModelSnapshot(QObject *parent)
	: QAbstractItemModel(parent)
{
}


void ItemModelSnapshot::setSnapshot(shared_ptr<Snapshot> sn) {
	snapshot = sn;
	rebuildIndexes();
}


void ItemModelSnapshot::rebuildIndexes() {
	parentMap.clear();
	if (!snapshot) return;
	for (auto root = snapshot->cbegin(); root != snapshot->cend(); ++root) {}
}


QVariant ItemModelSnapshot::headerData(int section, Qt::Orientation orientation, int role) const
{
	// FIXME: Implement me!
}


QModelIndex ItemModelSnapshot::index(int row, int column, const QModelIndex &parent) const
{
	// FIXME: Implement me!
}


QModelIndex ItemModelSnapshot::parent(const QModelIndex &index) const
{
	// FIXME: Implement me!
}


int ItemModelSnapshot::rowCount(const QModelIndex &parent) const
{
	if (!parent.isValid())
		return 0;

	// FIXME: Implement me!
}


int ItemModelSnapshot::columnCount(const QModelIndex &parent) const
{
	if (!parent.isValid())
		return 0;

	// FIXME: Implement me!
}


QVariant ItemModelSnapshot::data(const QModelIndex &index, int role) const
{
	if (!index.isValid())
		return QVariant();

	// FIXME: Implement me!
	return QVariant();
}
