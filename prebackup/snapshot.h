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
	void scan();
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
	void save(std::string const &file);  // throws
	static Snapshot* load(std::string const &file);  // throws

	// convenience functions
	static std::string sizeToText(uint64_t);

private:
	time_t timestamp;
	VectorOfPointers<Directory> rootDirs;
	uint64_t totSize = 0;
	bool needSave = false;  // whether it can/should be saved

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
