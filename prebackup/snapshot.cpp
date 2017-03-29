#include "snapshot.h"
#include <sstream>
#include <iomanip>
#include <memory>
using namespace std;


runtime_error Snapshot::saveError("Unable to save file");


Snapshot::Snapshot(vector<string> const &roots):
	timestamp(time(nullptr))
{
	for (string const &r: roots) rootDirs.push_back(new Directory(r, nullptr));
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


void Snapshot::sortSubDirs(std::function<bool(const Directory*, const Directory*)> func) {
	for (Directory *d: rootDirs) d->sortSubDirs(func);
}


void Snapshot::save(std::string const &file) {
	sqlite3 *db;
	int r = sqlite3_open(file.c_str(), &db);
	shared_ptr<sqlite3> finalizeDb(db, sqlite3_close);  // free object automatically
	if (r != SQLITE_OK) throw saveError;
	doQuery(db, "drop table if exists dir");
	doQuery(db, "create table dir (id INTEGER, name TEXT, parent INTEGER, "
		"filesize INTEGER, totsize INTEGER, excluded INTEGER, errors INTEGER)");
	doQuery(db, "create index index_dir on dir (id)");
	doQuery(db, "begin");
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


void Snapshot::saveRecur(InsertData &ins, int parentId, std::vector<Directory*>::const_iterator b, std::vector<Directory*>::const_iterator e) {
	for (; b != e; ++b) {
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
