#include "mainwindow.h"
#include <QApplication>
#include "snapshot.h"
#include <iostream>
#include <memory>
using namespace  std;




int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	QCoreApplication::setOrganizationName("prebackup");
	QCoreApplication::setApplicationName("prebackup");

	MainWindow w;
	w.show();
	return a.exec();
}
