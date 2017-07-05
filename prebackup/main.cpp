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


#include "mainwindow.h"
#include <QApplication>
#include <QTranslator>
#include <QLibraryInfo>
using namespace  std;




int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	QCoreApplication::setOrganizationName("prebackup");
	QCoreApplication::setApplicationName("prebackup");

	QTranslator qtTranslator;
	qtTranslator.load("qt_" + QLocale::system().name(),
		QLibraryInfo::location(QLibraryInfo::TranslationsPath));
	a.installTranslator(&qtTranslator);

	QTranslator myappTranslator;
	myappTranslator.load("prebackup_" + QLocale::system().name(),
			QCoreApplication::applicationDirPath() + "/../prebackup/tran")  // development
		|| myappTranslator.load("prebackup_" + QLocale::system().name(),
			"/usr/share/prebackup");
	a.installTranslator(&myappTranslator);

	MainWindow w;
	w.show();
	return a.exec();
}
