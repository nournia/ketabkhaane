#include <QtCore/QCoreApplication>


#include <AccessToSqlite.h>
#include <ReghaabatSync.h>

int main(int argc, char *argv[])
{    
    QCoreApplication a(argc, argv);

    //convertAccessDbToSqliteDb("D:\\Flash\\Project\\Match\\Match\\ForConvert.mdb", "reghaabat.db");

    syncDb();

    return a.exec();
}
