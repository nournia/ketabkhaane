#include <QtCore/QCoreApplication>

//#include <AccessToSqlite.h>
#include <syncer.h>
#include <connector.h>

#include <QDebug>
int main(int argc, char *argv[])
{    
    QCoreApplication a(argc, argv);

    //convertAccessDbToSqliteDb("D:\\Flash\\Project\\Match\\Match\\ForConvert.mdb", "reghaabat.db");

    QString json = Syncer::syncDb();
    Sender s;
    s.send(QUrl("http://localhost:3000/client/upload"), json);


    return a.exec();
}

