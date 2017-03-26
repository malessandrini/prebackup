#include "itemModelSnapshot.h"
#include "snapshot.h"
#include "directory.h"
#include <QStringList>
#include <algorithm>
using namespace std;


static QStringList columns = QStringList() << "name" << "tot size" << "file size";


ItemModelSnapshot::ItemModelSnapshot(QObject *parent)
	: QAbstractItemModel(parent)
{
}


void ItemModelSnapshot::setSnapshot(shared_ptr<Snapshot> sn) {
	snapshot = sn;
}


QVariant ItemModelSnapshot::headerData(int section, Qt::Orientation orientation, int role) const
{
	if (orientation == Qt::Horizontal && role == Qt::DisplayRole && section < columns.size()) return columns[section];
	return QVariant();
}


QModelIndex ItemModelSnapshot::index(int row, int column, const QModelIndex &parent) const
{
	vector<Directory>::const_iterator b, e;
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
	if (row >= (e - b) || column >= columns.size()) return QModelIndex();
	return createIndex(row, column, const_cast<Directory*>(&*(b + row)));
}


QModelIndex ItemModelSnapshot::parent(const QModelIndex &index) const
{
	if (!index.isValid()) return QModelIndex();
	const Directory *dir = static_cast<const Directory*>(index.internalPointer()), *parentDir;
	if (!dir || !(parentDir = dir->getParent())) return QModelIndex();
	// we have to find the parent's row in parent's parent's vector!
	vector<Directory>::const_iterator b, e;
	if (parentDir->getParent()) {
		 b = parentDir->getParent()->cbegin();
		 e = parentDir->getParent()->cend();
	}
	else {
		b = snapshot->cbegin();
		e = snapshot->cend();
	}
	auto i = std::find_if(b, e, [parentDir](const Directory &a){ return parentDir == &a; });
	return createIndex(i == e ? /*should not happen*/ 0 : (i - b), 0, const_cast<Directory*>(dir->getParent()));
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
	return columns.size();
}


QVariant ItemModelSnapshot::data(const QModelIndex &index, int role) const
{
	if (!index.isValid() || index.column() >= columns.size()) return QVariant();
	const Directory *dir = static_cast<const Directory*>(index.internalPointer());
	if (!dir) return QVariant();

}
