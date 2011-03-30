#include <helper.h>

StrMap getRecord(QSqlQuery& query)
{
    StrMap map;
    for (int i = 0; i < query.record().count(); i++)
        map[query.record().fieldName(i)] = query.value(i).toString();
    return map;
}

