Prebackup
=========

_Organize your directories before a backup_

Prebackup is not a backup program, but a tool you use before
your actual backup, to better organize your directories and
set what will and will not end up in your backup.

Usually when you backup your home or a portion of it, many files
are included that don't need a backup, especially large files
like videos, ISO installation images, installed games, etc.

Prebackup lets you fine-tune the list of directories and
exclusions in several ways:
- Scan a "snapshot" of the directories you want to backup,
  with information on the size of each directory and subdirectory
- Set directories to be excluded and rescan a new snapshot
  (\* see note)
- Save a snapshot for future reference
- Compare the current snapshot with a previously saved one,
  to check if and where abnormal increases in size have been
  introduced since the last backup, and possibly exclude new
  directories
- Generate output files to be used by your backup program,
  like a list of excluded directories

\* Note: currently directories are considered to be excluded
if they contain a file named `_nobackup_`, see full
documentation for details.


Installation
============

The program currently works on GNU/Linux and other Unix-like
systems. It's written with Qt toolkit, so provided you have all
the Qt development libraries and headers installed, there are
two ways to compile it:
- From command-line, go to the `prebackup` subdirectory where
  the `prebackup.pro` file is, and run `qmake` and then `make`
- Or using the Qt Creator IDE, open the project by opening the
  `prebackup.pro` file, then build and run

(Other libraries needed: sqlite3)


Usage
=====

See the Wiki page on GitHub:
<https://github.com/malessandrini/prebackup/wiki>
or click Help -> Documentation in the program.


License
=======

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

