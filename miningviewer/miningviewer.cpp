#include <iostream>
#include <string>

#include <sqlite3.h>

#include <QApplication>
#include <QFile>
#include <QRegExp>
#include <QString>
#include <QSqlDatabase>
#include <QSqlDriver>
#include <QString>

#include <QtDebug>

#include "MinerMainWindow.hh"

using namespace std;

extern "C" {
    static void regexpFun(sqlite3_context *ctx, int argc, sqlite3_value **argv) {
        if (argc < 2)
            return;

        uchar const *lhs = sqlite3_value_text(argv[1]);
        if (lhs == 0)
            return;
        QString text(QString::fromUtf8(reinterpret_cast<char const *>(lhs)));

        uchar const *rhs = sqlite3_value_text(argv[0]);
        if (rhs == 0)
            return;
        QString expr(QString::fromUtf8(reinterpret_cast<char const *>(rhs)));

        QRegExp regexp(expr);
        if (text.contains(regexp))
            sqlite3_result_int(ctx, 1);
        else
            sqlite3_result_int(ctx, 0);
    }
}


bool openDatabase(QString const &dbFilename)
{
	QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
	db.setDatabaseName(dbFilename);
    if (!db.open())
        return false;

    // Get the sqlite3 database handle
    QVariant handleV = db.driver()->handle();
    if (handleV.isValid() && qstrcmp(handleV.typeName(), "sqlite3*") == 0) {
         sqlite3 *handle = *static_cast<sqlite3 **>(handleV.data());
         if (handle != 0)
             if (sqlite3_create_function(handle, "regexp", -1, SQLITE_ANY, 0, regexpFun, 0, 0) != SQLITE_OK)
                 qWarning() << "Could not register REGEXP operator with sqlite!";
     }

    return true;
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
