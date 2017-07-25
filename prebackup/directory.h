/*
Prebackup
Copyright (C) 2017  Michele Alessandrini

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License version 3
as published by the Free Software Foundation.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/


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
	Directory(Directory const&) = delete;
	Directory& operator=(Directory const&) = delete;
	std::string getName() const { return name; }
	const Directory * getParent() const { return parent; }
	uint64_t getFileSize() const { return fileSize; }
	uint64_t getTotSize() const { return totSize; }
	int64_t getDiffSize() const { return diffSize; }
	bool isExcluded() const { return excluded; }
	bool hasErrors() const { return errors; }
	bool isGhost() const { return ghost; }
	bool isAdded() const { return added; }
	// subdirectories
	std::vector<Directory*>::const_iterator cbegin() const { return subDirs.cbegin(); }
	std::vector<Directory*>::const_iterator cend()   const { return subDirs.cend(); }
	// sort subdirectories (invalidates iterators)
	void sortSubDirs(std::function<bool(const Directory*, const Directory*)>);
	// exclude marker
	static const std::string defaultExcludeMarker;
	static void setExcludeMarker(const std::string &);
private:
	const std::string name;
	const Directory * const parent;
	uint64_t fileSize = 0;
	uint64_t totSize = 0;
	int64_t diffSize = 0;
	bool excluded = false;
	bool errors = false;
	bool ghost = false;  // existing in compared snapshot but not in this one
	bool added = false;  // existing in this snapshot but not in compared one
	VectorOfPointers<Directory> subDirs;
	friend class Snapshot;  // to save/load from file easily
private:
	static std::string excludeMarker;
	void scan(std::string const &parentPath);
	void clear();
};


#endif // DIRECTORY_H
