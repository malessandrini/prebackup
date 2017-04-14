#ifndef SNAPSHOT_H
#define SNAPSHOT_H


#include <cstdint>
#include <vector>
#include <string>
#include <ctime>
#include <stdexcept>
#include <sqlite3.h>
#include "directory.h"


class Snapshot {
public:
	Snapshot(std::vector<std::string> const &roots);
	Snapshot(Snapshot const&) = delete;
	Snapshot& operator=(Snapshot const&) = delete;
	time_t getTimestamp() const { return timestamp; }
	uint64_t getTotSize() const { return totSize; }
	bool isSaved() const { return !needSave; }
	bool isEmpty() const { return !rootDirs.size(); }

	// root directories
	std::vector<Directory*>::const_iterator cbegin() const { return rootDirs.cbegin(); }
	std::vector<Directory*>::const_iterator cend()   const { return rootDirs.cend(); }

	// sort subdirectories (invalidates iterators)
	void sortSubDirs(std::function<bool(const Directory*, const Directory*)>);

	// load/save to file
	void save(std::string const &file) const;  // throws
	static Snapshot* load(std::string const &file);  // throws

	void recomputeDifference(const Snapshot *other);

	// convenience functions
	static std::string sizeToText(uint64_t);
	static std::string relSizeToText(int64_t);

private:
	time_t timestamp;
	VectorOfPointers<Directory> rootDirs;
	uint64_t totSize = 0;
	mutable bool needSave = false;  // whether it can/should be saved
	void scan();
	static void removeGhosts(VectorOfPointers<Directory>&);
	static void compareRecursive(const Directory *parent, VectorOfPointers<Directory>&, const VectorOfPointers<Directory>&);

	// data and functions for sqlite db read/write
	static void doQuery(sqlite3*, const char*);  // throws
	static std::runtime_error saveError;
	struct InsertData {
		sqlite3_stmt *query;
		uint32_t id;
		int bindId, bindName, bindParent, bindFilesize, bindTotsize, bindExcluded, bindErrors;
	};
	static void saveRecur(InsertData &, uint32_t parentId, std::vector<Directory*>::const_iterator, std::vector<Directory*>::const_iterator);  // throws
};


#endif // SNAPSHOT_H
