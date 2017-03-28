#ifndef SNAPSHOT_H
#define SNAPSHOT_H


#include <cstdint>
#include <vector>
#include <string>
#include <ctime>
#include "directory.h"


class Snapshot {
public:
	Snapshot(std::vector<std::string> const &roots);
	void scan();
	time_t getTimestamp() const { return timestamp; }
	uint64_t getTotSize() const { return totSize; }
	// root directories
	std::vector<Directory*>::const_iterator cbegin() const { return rootDirs.cbegin(); }
	std::vector<Directory*>::const_iterator cend()   const { return rootDirs.cend(); }
	// sort subdirectories (invalidates iterators)
	void sortSubDirs(std::function<bool(const Directory*, const Directory*)>);
	// convenience functions
	static std::string sizeToText(uint64_t);
private:
	time_t timestamp;
	VectorOfPointers<Directory> rootDirs;
	uint64_t totSize = 0;
};


#endif // SNAPSHOT_H
