#include <iostream>
#include <string>

#include <QApplication>
#include <QFile>
#include <QString>
#include <QSqlDatabase>

#include "MinerMainWindow.hh"

using namespace std;

bool openDatabase(QString const &dbFilename)
{
	QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
	db.setDatabaseName(dbFilename);
	return db.open();
}

void usage(string const &programName)
{
	cout << "Syntax: " << programName << " mine_database" << endl;
}

int main(int argc, char *argv[])
{
	if (argc != 2)
	{
		usage(argv[0]);
		return 1;
	}

	QApplication app(argc, argv);
	
	QString dbFilename = argv[1];
	if (!QFile::exists(dbFilename)) {
		cout << "File does not exist: " <<
			dbFilename.toLatin1().constData() << endl;
		return 1;
	}
		
	if (!openDatabase(dbFilename)) {
		cout << "Error opening: " << dbFilename.toLatin1().constData() <<
			endl;
		return 1;
	}
	
	miningviewer::MinerMainWindow mainWindow;
	mainWindow.show();
	return app.exec();
}
