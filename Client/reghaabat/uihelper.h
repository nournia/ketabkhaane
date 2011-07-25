#ifndef UIHELPER_H
#define UIHELPER_H

#include <QComboBox>
#include <QTableView>
#include <QHeaderView>

#include <helper.h>

void fillComboBox(QComboBox* combobox, QList<StrPair> data);
void customizeTable(QTableView* table, int columns, int width = 80, bool sort = true, int stretch = 1, bool header = false);

#endif // UIHELPER_H
