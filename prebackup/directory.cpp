#include "directory.h"
#include <algorithm>
#include <dirent.h>
#include <errno.h>
#include <cstring>
#include <sys/stat.h>
#include <memory>
using namespace std;


const string Directory::excludeMarker("_nobackup_");


Directory::Directory(string const &_name, const Directory *_parent):
	name(_name), parent(_parent)
{}


void Directory::clear() {
	fileSize = 0;
	totSize = 0;
	excluded = false;
	errors = false;
	subDirs.clear();
}


void Directory::scan(string const &parentPath) {
	clear();
	string dirFullPath = parentPath.size() ? ( parentPath + "/" + name) : name;
	DIR *d = opendir(dirFullPath.c_str());
	if (!d) {
		errors = true;
		return;
	}
	shared_ptr<DIR> cleanup(d, closedir);  // automatically close d
	dirent *entry;
	vector<string> dirNames;
	do {
		errno = 0;
		entry = readdir(d);
		if (!entry) {
			if (errno) errors = true;
			break;  // no more entries
		}
		if (!strcmp(entry->d_name, ".") || !strcmp(entry->d_name, "..")) continue;
		string entryFullPath = dirFullPath + "/" + entry->d_name;
		struct stat64 fileStat;
		if (lstat64(entryFullPath.c_str(), &fileStat)) {
			errors = true;
			continue;
		}
		if (S_ISREG(fileStat.st_mode) || S_ISLNK(fileStat.st_mode)) {
			// regular file or symbolic link
			if (excludeMarker == entry->d_name && S_ISREG(fileStat.st_mode)) {
				clear();
				excluded = true;
				return;
			}
			fileSize += fileStat.st_size;
			totSize += fileStat.st_size;
		}
		else if (S_ISDIR(fileStat.st_mode)) {
			// directory
			// save all directories' names for later, so we don't waste
			// time scanning them if we find that this whole directory
			// has to be excluded.
			dirNames.push_back(entry->d_name);
		}
	} while(true);
	// scan subdirectories
	for (string const &subd: dirNames) {
		subDirs.push_back(new Directory(subd, this));
		subDirs.back()->scan(dirFullPath);
		totSize += subDirs.back()->getTotSize();
		errors = errors || subDirs.back()->hasErrors();
	}
}


void Directory::sortSubDirs(std::function<bool(const Directory*, const Directory*)> func) {
	std::sort(subDirs.begin(), subDirs.end(), func);
	for (Directory *d: subDirs) d->sortSubDirs(func);
}
