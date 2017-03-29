#ifndef DIRECTORY_H
#define DIRECTORY_H


#include <cstdint>
#include <vector>
#include <string>
#include <functional>


template<class T>
class VectorOfPointers: public std::vector<T*> {
public:
	~VectorOfPointers() { for (T* item: *this) delete item; }
};


class Directory {
public:
	Directory(std::string const &name, const Directory *parent);
	void scan(std::string const &parentPath);
	std::string getName() const { return name; }
	const Directory * getParent() const { return parent; }
	uint64_t getFileSize() const { return fileSize; }
	uint64_t getTotSize() const { return totSize; }
	bool isExcluded() const { return excluded; }
	bool hasErrors() const { return errors; }
	// subdirectories
	std::vector<Directory*>::const_iterator cbegin() const { return subDirs.cbegin(); }
	std::vector<Directory*>::const_iterator cend()   const { return subDirs.cend(); }
	// sort subdirectories (invalidates iterators)
	void sortSubDirs(std::function<bool(const Directory*, const Directory*)>);
private:
	const std::string name;
	const Directory * const parent;
	uint64_t fileSize = 0;
	uint64_t totSize = 0;
	bool excluded = false;
	bool errors = false;
	VectorOfPointers<Directory> subDirs;
	friend class Snapshot;  // to save/load from file easily
private:
	static const std::string excludeMarker;
	void clear();
};


#endif // DIRECTORY_H
