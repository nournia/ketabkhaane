#include <QtCore/QCoreApplication>

#include <AccessToSqlite.h>
#include <syncer.h>
#include <connector.h>

#include <QCryptographicHash>

#include <QDebug>
int main(int argc, char *argv[])
{    
    QCoreApplication a(argc, argv);

    convertAccessDbToSqliteDb("D:\\Flash\\Project\\Match\\Match\\ForConvert.mdb", "reghaabat.db");

    Sender sender;
    sender.sync();

    return a.exec();
}

