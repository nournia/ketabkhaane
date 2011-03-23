#include <QtCore/QCoreApplication>

#include <AccessToSqlite.h>
#include <syncer.h>
#include <connector.h>

#include <QDebug>
int main(int argc, char *argv[])
{    
    QCoreApplication a(argc, argv);

    //convertAccessDbToSqliteDb("D:\\Flash\\Project\\Match\\Match\\ForConvert.mdb", "reghaabat.db");

    Syncer syncer;
    Sender sender;

    QString json = syncer.getChunk();
    //while (! json.isNull())
    {
        sender.send(QUrl("http://localhost/server.php"), json);
    }

/*
    QSqlQuery qry;

    if (! qry.exec("update matches set id = 12 where id = 311006"))
        qDebug() << qry.lastError();

    qDebug() << "ok";
*/

    return a.exec();
}

