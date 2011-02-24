#include <QtCore/QCoreApplication>
#include <AccessToSqlite.h>

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    new AccessToSqlite("D:\\Flash\\Project\\Match\\Match\\ForConvert.mdb", "reghaabat.db");

    return a.exec();
}
