#include "mobjects.h"

bool MObjects::get(QString objectId, StrMap& object)
{
    QSqlQuery qry;
    qry.exec("select objects.*, authors.title as author, publications.title as publication, branches.root_id "
             "from objects left join authors on objects.author_id = authors.id left join publications on objects.publication_id = publications.id left join branches on objects.branch_id = branches.id where objects.id = "+ objectId);
    if (! qry.next()) return false;

    object = getRecord(qry);
    return true;
}

QString MObjects::getNewLabel(QString branch)
{
    QSqlQuery qry;

    QString label;
    qry.exec("select label from branches where id = "+ branch);
    if (qry.next())
        label = qry.value(0).toString();

    int number = 0;
    qry.exec("select label from objects where branch_id = "+ branch +" order by label desc limit 1");
    if (qry.next())
        if (qry.value(0).toString().left(label.length()) == label)
            number = qry.value(0).toString().mid(label.length()+1).toInt();

    QString result;
    result.sprintf("%3d", number + 1);
    result = label +"-"+ result.replace(" ", "0");
    return result;
}
