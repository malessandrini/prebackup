#include "snapshot.h"
#include <sstream>
#include <iomanip>
#include <memory>
#include <map>
#include <set>
using namespace std;


runtime_error Snapshot::saveError("Unable to save file");


Snapshot::Snapshot(vector<string> const &roots):
	timestamp(time(nullptr))
{
	for (string const &r: roots) rootDirs.push_back(new Directory(r, nullptr));
	if (rootDirs.size()) scan();
}


void Snapshot::scan() {
	totSize = 0;
	for (auto *d: rootDirs) {
		d->scan("");
		totSize += d->getTotSize();
	}
	timestamp = time(nullptr);
	needSave = true;
}


string Snapshot::sizeToText(uint64_t size) {
	if (!size) return "0";
	static const char * units[] = { "B", "KiB", "MiB", "GiB", "TiB", nullptr };  // "640 TiB ought to be enough for anybody"
	float s = size;
	const char ** u = units;
	while (*u && *(u + 1)) {
		if (s < 1000) break;  // let's choose 1000 as threshold
		s /= 1024;
		++u;
	}
	ostringstream oss;
	oss << fixed << setprecision(u == units ? 0 : 1) << s << " " << *u;
	return oss.str();
}


string Snapshot::relSizeToText(int64_t size) {
	if (!size) return "0";
	if (size > 0) return "+" + sizeToText(size);
	return "-" + sizeToText(-size);
}


void Snapshot::sortSubDirs(std::function<bool(const Directory*, const Directory*)> func) {
	for (Directory *d: rootDirs) d->sortSubDirs(func);
}


void Snapshot::save(std::string const &file) const {
	sqlite3 *db;
	int r = sqlite3_open(file.c_str(), &db);
	shared_ptr<sqlite3> finalizeDb(db, sqlite3_close);  // free object automatically
	if (r != SQLITE_OK) throw saveError;
	doQuery(db, "drop table if exists dir");
	doQuery(db, "create table dir (id INTEGER, name TEXT, parent INTEGER, "
		"filesize INTEGER, totsize INTEGER, excluded INTEGER, errors INTEGER)");
	doQuery(db, "create index index_dir on dir (id)");
	doQuery(db, "begin");
	// use row with id 0 to save snapshot's general information
	ostringstream oss;
	oss << "insert into dir (id, filesize, totsize) values (0, " << timestamp << ", " << totSize << ")";
	string s = oss.str();
	doQuery(db, s.c_str());
	// recurse directories
	InsertData ins;
	ins.id = 1;  // next id
	if (sqlite3_prepare_v2(db, "insert into dir (id, name, parent, filesize, totsize, excluded, errors) "
		"values (:id, :name, :parent, :filesize, :totsize, :excluded, :errors)", -1, &ins.query, nullptr) != SQLITE_OK)
			throw saveError;
	shared_ptr<sqlite3_stmt> finalizeQuery(ins.query, sqlite3_finalize);  // free object automatically
	ins.bindId = sqlite3_bind_parameter_index(ins.query, ":id");
	ins.bindName = sqlite3_bind_parameter_index(ins.query, ":name");
	ins.bindParent = sqlite3_bind_parameter_index(ins.query, ":parent");
	ins.bindFilesize = sqlite3_bind_parameter_index(ins.query, ":filesize");
	ins.bindTotsize = sqlite3_bind_parameter_index(ins.query, ":totsize");
	ins.bindExcluded = sqlite3_bind_parameter_index(ins.query, ":excluded");
	ins.bindErrors = sqlite3_bind_parameter_index(ins.query, ":errors");
	saveRecur(ins, 0, rootDirs.cbegin(), rootDirs.cend());
	finalizeQuery.reset();  // actually finalize query
	doQuery(db, "commit");
	needSave = false;
}


void Snapshot::doQuery(sqlite3 *db, const char *query) {
	if (sqlite3_exec(db, query, nullptr, nullptr, nullptr) != SQLITE_OK) throw saveError;
}


void Snapshot::saveRecur(InsertData &ins, uint32_t parentId, std::vector<Directory*>::const_iterator b, std::vector<Directory*>::const_iterator e) {
	for (; b != e; ++b) {
		if ((*b)->ghost) continue;
		// insert row for this directory
		int id = ins.id++;
		if (
			sqlite3_bind_int(ins.query, ins.bindId, id) != SQLITE_OK
			|| sqlite3_bind_text(ins.query, ins.bindName, (*b)->name.c_str(), -1, SQLITE_STATIC) != SQLITE_OK
			|| sqlite3_bind_int(ins.query, ins.bindParent, parentId) != SQLITE_OK
			|| sqlite3_bind_int64(ins.query, ins.bindFilesize, (*b)->fileSize) != SQLITE_OK
			|| sqlite3_bind_int64(ins.query, ins.bindTotsize, (*b)->totSize) != SQLITE_OK
			|| sqlite3_bind_int(ins.query, ins.bindExcluded, (*b)->excluded) != SQLITE_OK
			|| sqlite3_bind_int(ins.query, ins.bindErrors, (*b)->errors) != SQLITE_OK
		) throw saveError;
		if (sqlite3_step(ins.query) != SQLITE_DONE) throw saveError;
		sqlite3_reset(ins.query);
		// recurse subdirs of current directory
		saveRecur(ins, id, (*b)->cbegin(), (*b)->cend());
	}
}


Snapshot * Snapshot::load(string const &file) {
	unique_ptr<Snapshot> snap(new Snapshot({}));  // delete object automatically in case of exception
	runtime_error loadError("Unable to load file");
	sqlite3 *db;
	int r = sqlite3_open_v2(file.c_str(), &db, SQLITE_OPEN_READONLY, nullptr);
	shared_ptr<sqlite3> finalizeDb(db, sqlite3_close);  // free object automatically
	if (r != SQLITE_OK) throw loadError;
	sqlite3_stmt *query;
	if (sqlite3_prepare_v2(db, "select id, name, parent, filesize, totsize, excluded, errors from dir order by id",
		-1, &query, nullptr) != SQLITE_OK) throw loadError;
	shared_ptr<sqlite3_stmt> finalizeQuery(query, sqlite3_finalize);  // free object automatically
	map<uint32_t, Directory*> idMap;
	bool firstRow = true;
	while ((r = sqlite3_step(query)) == SQLITE_ROW) {
		uint32_t id = sqlite3_column_int(query, 0);
		const unsigned char *name = sqlite3_column_text(query, 1);
		uint32_t parentId = sqlite3_column_int(query, 2);
		uint64_t fileSize = sqlite3_column_int64(query, 3);
		uint64_t totSize = sqlite3_column_int64(query, 4);
		bool excluded = sqlite3_column_int(query, 5);
		bool errors = sqlite3_column_int(query, 6);
		if (firstRow) {
			if (id != 0) throw loadError;
			firstRow = false;
			snap->timestamp = fileSize;
			snap->totSize = totSize;
			continue;
		}
		Directory *parentDir = nullptr;
		if (parentId) {
			auto p = idMap.find(parentId);
			if (p == idMap.end()) throw loadError;
			parentDir = p->second;
		}
		Directory *dir = new Directory((const char*)name, parentDir);
		idMap[id] = dir;
		if (parentDir) parentDir->subDirs.push_back(dir);
		else snap->rootDirs.push_back(dir);
		dir->fileSize = fileSize;
		dir->totSize = totSize;
		dir->excluded = excluded;
		dir->errors = errors;
	}
	if (r != SQLITE_DONE) throw loadError;
	// success, return newly created object
	return snap.release();
}


void Snapshot::removeGhosts(VectorOfPointers<Directory> &v) {
	for (unsigned i = 0; i < v.size(); )
		if (v[i]->ghost) {
			delete v[i];
			v.erase(v.cbegin() + i);
		}
		else ++i;
	for (Directory *d: v) removeGhosts(d->subDirs);
}


void Snapshot::recomputeDifference(const Snapshot *other) {
	removeGhosts(rootDirs);  // directories previously marked as deleted
	compareRecursive(nullptr, rootDirs, other->rootDirs);
}


void Snapshot::compareRecursive(const Directory *parent, VectorOfPointers<Directory> &v1, const VectorOfPointers<Directory> &v2) {
	static const VectorOfPointers<Directory> dummy;  // for when second directory does not exist
	set<string> set1;  // list of names in v1
	for (const Directory *d: v1) set1.insert(d->name);
	map<string, const Directory*> map2;  // mapping names->dirs in v2
	for (const Directory *d: v2) map2.insert({d->name, d});
	// find directories in v2 not existing in v1, and create ghost dirs in v1 for them
	for (const auto &d2: map2)
		if (set1.find(d2.first) == set1.end()) {
			v1.push_back(new Directory(d2.first, parent));
			v1.back()->ghost = true;
		}
	// compute size difference
	for (Directory *d1: v1) {
		uint64_t size1 = d1->totSize, size2;
		auto d2 = map2.find(d1->name);
		if (d2 == map2.end()) {
			size2 = 0;
			d1->added = true;
		}
		else size2 = d2->second->totSize;
		d1->diffSize = (int64_t)size1 - (int64_t)size2;
		// recurse subdirectories
		compareRecursive(d1, d1->subDirs, (d2 == map2.end() ? dummy : d2->second->subDirs));
	}
}
