#ifndef DIRECTORY_H
#define DIRECTORY_H


#include <cstdint>
#include <vector>
#include <string>
#include <functional>


class Directory {
public:
	Directory(std::string const &name);
	void scan(std::string const &parentPath);
	std::string getName() const { return name; }
	uint64_t getFileSize() const { return fileSize; }
	uint64_t getTotSize() const { return totSize; }
	bool isExcluded() const { return excluded; }
	bool hasErrors() const { return errors; }
	// subdirectories
	std::vector<Directory>::const_iterator cbegin() const { return subDirs.cbegin(); }
	std::vector<Directory>::const_iterator cend()   const { return subDirs.cend(); }
	// sort subdirectories (invalidates iterators)
	void sortSubDirs(std::function<bool(const Directory&, const Directory&)>);
private:
	std::string name;
	uint64_t fileSize = 0;
	uint64_t totSize = 0;
	bool excluded = false;
	bool errors = false;
	std::vector<Directory> subDirs;
private:
	static const std::string excludeMarker;
	void clear();
};


#endif // DIRECTORY_H
