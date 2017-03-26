#include "snapshot.h"
#include "directory.h"
#include <sstream>
#include <iomanip>
using namespace std;


Snapshot::Snapshot(vector<string> const &roots):
	timestamp(time(nullptr))
{
	for (string const &r: roots) rootDirs.push_back(Directory(r, nullptr));
}


void Snapshot::scan() {
	totSize = 0;
	for (auto &d: rootDirs) {
		d.scan("");
		totSize += d.getTotSize();
	}
	timestamp = time(nullptr);
}


string Snapshot::sizeToText(uint64_t size) {
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
